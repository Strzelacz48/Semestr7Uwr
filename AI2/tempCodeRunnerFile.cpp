//pomocnicza do debugu
void BprintRow(int* newRow, int i, int size){
    cerr<<"\nRow : "<<i<<endl;
    cerr<<"Before moveRow"<<endl;
    for(int j = 0; j < size; j++){
        cerr<<newRow[j]<<" ";
    }
    cerr<<endl;
}

//pomocnicza do debugu
void AprintRow(int* newRow, int i, int size){
    cerr<<"\nAfter moveRow"<<endl;
    for(int j = 0; j < size; j++){
        cerr<<newRow[j]<<" ";
    }
    cerr<<endl;
}