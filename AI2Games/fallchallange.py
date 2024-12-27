import sys
import math
import numpy
from concurrent.futures import ThreadPoolExecutor, TimeoutError
import time
import queue

first_round = True
output = ""
travel_routes = []
pods = []
existing_buildings = {}
unconnected_buildings = []
unconnected_buildingsd = {}
nextpod = 1
#Plansza ma 160x90 km
# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.
def distance(p1, p2):
    return math.sqrt((p2.coord_x-p1.coord_x)**2 + (p2.coord_y - p1.coord_y)**2)

def pointOnSegment(A, B, C) :
    epsilon = 0.0000001
    return (-epsilon < distance(B, A) + distance(A, C) - distance(B, C) < epsilon)

def orientation(p1, p2, p3) :
    prod = (p3.coord_y-p1.coord_y) * (p2.coord_x-p1.coord_x) - (p2.coord_y-p1.coord_y) * (p3.coord_x-p1.coord_x)
    return numpy.sign(prod)

def segmentsIntersect(A, B, C, D) :
    return orientation(A, B, C) * orientation(A, B, D) < 0 and orientation(C, D, A) * orientation(C, D, B) < 0

def tubeCost(A, B) :
    return distance(A,B) * 0.1
#BUILD FUNCTIONS
#================================================================================================================================

def buildTeleport(buildingIdEntrance, buildingIdExit):
    return "TELEPORT "+str(buildingIdEntrance)+" "+str(buildingIdExit) + ";"

def buildTube(buildingId1, buildingId2):
    return "TUBE "+str(buildingId1)+" "+str(buildingId2) + ";"

def upgradeTube(buildingId1, buildingId2):
    return "UPGRADE "+str(buildingId1)+" "+str(buildingId2) + ";"

def buildPod(podId, route):
    result = "POD "+str(podId)
    for stop in route:
        result = result+" "+str(stop)
    return result + ";"

def destroy(podId):
    return "DESTROY "+str(podId) + ";"

def waiting():
    return "WAIT"

#================================================================================================================================

# Powinno się dać teleporty wszędzie w kółko, a potem łączyć nowo zrobione grafy krawędzią
def greedytp(output, resources):
        #max_distance = 0
    while resources >= 5000:
        start_time = time.time()
        if len(unconnected_buildingsd[0]) > 0:
            start = unconnected_buildingsd[0][0]
            #print("Start : ", start, file=sys.stderr, flush=True)
        else:
            return output, resources
        #unconnected_buildings.index

        maks_astr = float('-inf')
        maks_idx = None
        #print ("unconnected_buildings", unconnected_buildingsd, file=sys.stderr, flush=True)
        for klucz in start.astronauts.keys():
            liczbaAstronaut = start.astronauts[klucz]
            #print ("Klucz", klucz, file=sys.stderr, flush=True)
            if liczbaAstronaut > maks_astr and unconnected_buildingsd[klucz] is not None and len(unconnected_buildingsd[klucz]) > 0:
                maks_astr = liczbaAstronaut
                maks_idx = klucz

        #print("najwięcej jest astronautów o numerze : ", maks_idx, file=sys.stderr, flush=True)

        building = unconnected_buildingsd[maks_idx][0]
        output+= buildTeleport(start.building_id,building.building_id)
        travel_routes.append(Pipe(start.building_id, building.building_id, 0))
        unconnected_buildingsd[maks_idx].remove(building)
        unconnected_buildingsd[0].remove(start)
        start.teleport = True

        resources -= 5000
        end_time = time.time()
        execution_time = end_time - start_time
        print("Czas whilea : ", execution_time, file=sys.stderr, flush=True)
    #zbuduj teleport z lądowiska do najbardziej potrzebnego budynku dla tego lądowiska.
    #następnie spróbuj połączyć wyjście teleportu z wejściem innego teleportu
    return output, resources

def findBuildingById(buildingId):
    for building in existing_buildings:
        if building.building_id == buildingId:
            return building
    return None

def naive_pipes(output: str, resources: int, nextpod):
    if resources == 0:
        return output
    
    for landing_pad in existing_buildings[0] :
        if landing_pad.tubes == len(landing_pad.astronauts) or landing_pad.tubes >= 5:
            continue
        astronauts_nr_desc = landing_pad.get_sorted_astronaut_indices_descending()
        train_path = []
        i = 0
        train_path.append(landing_pad.building_id)
        #last_stop = landing_pad.building_id
        for type in astronauts_nr_desc :
            if i==0 and landing_pad.teleport :
                i += 1
                continue
            
            added = False
            for sink in existing_buildings[type] :
                if sink.tubes < 5 and resources - tubeCost(landing_pad, sink) >= 1000:
                    travel_routes.append(Pipe(landing_pad.building_id,sink.building_id,1))
                    resources-= tubeCost(landing_pad, sink)
                    sink.tubes += 1
                    output = output + buildTube(landing_pad.building_id, sink.building_id)
                    train_path.append(sink.building_id)
                    train_path.append(landing_pad.building_id)
                    added = True
                    break
            
        #if added :
        #    train_path.append(landing_pad.building_id)

        print ("Train path to add", train_path, file=sys.stderr, flush=True)
        output = output + buildPod(nextpod,train_path)
        nextpod += 1
        resources -= 1000
    return output, resources, nextpod
#==============================================================================================
# KLASY I WPROWADZANIE DANYCH
class Building:
    def __init__(self, building_type, building_id, coord_x, coord_y, astronauts=None):
        self.building_type = building_type
        self.building_id = building_id
        self.coord_x = coord_x
        self.coord_y = coord_y
        self.astronauts = astronauts or {}  # Słownik astronautów dla lądowiska
        self.tubes = 0
        self.teleport = False

    def __repr__(self):
        if self.building_type == 0:  # Lądowisko
            return (f"Landing Pad (ID: {self.building_id}, Coordinates: ({self.coord_x}, {self.coord_y}), "
                    f"Astronauts: {self.astronauts}, Tubes: {self.tubes})")
        else:  # Moduł księżycowy
            return (f"Lunar Module (Type: {self.building_type}, ID: {self.building_id}, "
                    f"Tubes: {self.tubes}, Coordinates: ({self.coord_x}, {self.coord_y}))")

    def get_sorted_astronaut_indices_descending(self):
        if self.building_type != 0:
            return []  # Funkcja ma sens tylko dla lądowisk

        # Sortujemy astronautów według ich liczby w nierosnącej kolejności (malejącej)
        sorted_astronauts = sorted(self.astronauts.items(), key=lambda x: x[1], reverse=True)
        # Zwracamy listę indeksów astronautów w odpowiedniej kolejności
        return [astronaut_type for astronaut_type, count in sorted_astronauts]

def process_building_data(existing_buildings=None, unconnected_buildingsd=None):
    # Używamy istniejącej listy budynków lub tworzymy nową, jeśli nie została podana
    if existing_buildings is None:
        existing_buildings = {}
    
    if unconnected_buildingsd is None:
        unconnected_buildingsd = {}
    #print("Przed inputem", file=sys.stderr, flush=True)
    # Pobieramy liczbę nowych budynków
    num_new_buildings = int(input()) #"Enter the number of new buildings: "

    for _ in range(num_new_buildings):
        building_data = list(map(int, input().split())) #"Enter building data: "
        #print("kolejny budynek", file=sys.stderr, flush=True)
        building_type =  building_data[0]
        # Rozpoznanie, czy to lądowisko czy moduł księżycowy
        if building_type == 0:  # Lądowisko
            building_id = building_data[1]
            coord_x = building_data[2]
            coord_y = building_data[3]
            num_astronauts = building_data[4]
            
            # Tworzymy słownik astronautów: klucz to typ astronauty, wartość to liczba astronautów tego typu
            astronauts = {}
            for j in range(5, 5 + num_astronauts):
                astronaut_type = building_data[j]
                if astronaut_type in astronauts:
                    astronauts[astronaut_type] += 1
                else:
                    astronauts[astronaut_type] = 1

            new_building = Building(building_type, building_id, coord_x, coord_y, astronauts)
            #buildings.append(Building(building_type, building_id, coord_x, coord_y, astronauts))
            #unconnected_buildings.append(Building(building_type, building_id, coord_x, coord_y, astronauts))

        else:  # Moduł księżycowy
            building_id = building_data[1]
            coord_x = building_data[2]
            coord_y = building_data[3]
            new_building = Building(building_type, building_id, coord_x, coord_y)
            #buildings.append(Building(building_type, building_id, coord_x, coord_y))
            #unconnected_buildings.append(Building(building_type, building_id, coord_x, coord_y))
        #print("Mamy typ budynku", file=sys.stderr, flush=True)
        # Dodajemy nowy budynek do odpowiedniej grupy w słowniku existing_buildings
        if building_type not in existing_buildings:
            existing_buildings[building_type] = []
        existing_buildings[building_type].append(new_building)
        #print("Drugi if zostal", file=sys.stderr, flush=True)
        if building_type not in unconnected_buildingsd:
            unconnected_buildingsd[building_type] = []
        unconnected_buildingsd[building_type].append(new_building)

    #print("existing_buildings : ",existing_buildings, file=sys.stderr, flush=True)
    #print("unconnected_buildingsd : ",unconnected_buildingsd, file=sys.stderr, flush=True)

    return existing_buildings, unconnected_buildingsd

class Pipe:
    def __init__(self, building_id_1, building_id_2, capacity):
        self.From = building_id_1
        self.To = building_id_2
        self.capacity = capacity # 0 to teleporter

    def __repr__(self):
        return (f"Magnetic Tube (From: {self.From}, To: {self.To}, "
                    f"Capacity: {self.capacity})")
#============================================================================================
# game loop
while True:
    if first_round :
        max_time = 1
    else:
        max_time = 0.5
    output = ""
    travel_routes = []
    #parsowanie waluty
    resources = int(input())

    #print("resources : ", resources, file=sys.stderr, flush=True)

    #parsowanie ścieżek ciuchć
    num_travel_routes = int(input())
    for i in range(num_travel_routes):
        building_id_1, building_id_2, capacity = [int(j) for j in input().split()]
        travel_routes.append(Pipe(building_id_1, building_id_2, capacity))
    
    #print("travel_routes table : ",travel_routes, file=sys.stderr, flush=True)

    #parsowanie ciuchć
    num_pods = int(input())
    for i in range(num_pods):
        pod_properties = input()
        pods.append(pod_properties)
    
    #print("pods table : ",pods, file=sys.stderr, flush=True)

    #parsowanie wejścia budynków
    existing_buildings, unconnected_buildingsd = process_building_data(existing_buildings, unconnected_buildingsd)
    #print("buildings table : ",existing_buildings, file=sys.stderr, flush=True)

    # Myślenie jak zwrócić
    wait = True
    #print("Resources before : ", resources,  file=sys.stderr, flush=True)
    if resources > 5000:
        before = resources
        output, resources = greedytp(output, resources)
        if not resources == before :
            wait = False
    if resources > 3000:
        before = resources
        output, resources, nextpod = naive_pipes(output, resources, nextpod)
        if not resources == before :
            wait = False
    if wait:
        output = "" + waiting()
    #print("Resources after : ", resources,  file=sys.stderr, flush=True)
    print(output)
    first_round = False
    # Write an action using print
    # To debug: print("Debug messages...", file=sys.stderr, flush=True)

    # TUBE | UPGRADE | TELEPORT | POD | DESTROY | WAIT
    #print("TUBE 0 1;TUBE 0 2;POD 42 0 1 0 2 0 1 0 2")
    #break