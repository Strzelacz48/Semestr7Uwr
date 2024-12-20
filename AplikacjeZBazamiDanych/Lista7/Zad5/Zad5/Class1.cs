using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Zad5
{
    public class Person
    {
        public int Id { get; set; }
        public string Name { get; set; }
    }

    public class Movie
    {
        public int Id { get; set; }
        public string Title { get; set; }
    }

    public class ActedIn
    {
        public Person Actor { get; set; }
        public Movie Movie { get; set; }
        public string Role { get; set; }
    }

}
