

#include <iostream>
#include <string>

using namespace std;

class Movie {
    private:
        string title;
        unsigned int year;
        string genre;

    public:
        string gettitle() {
            return title;
        }

        unsigned int getYear() {
            return year;
        }

        string getGenre() {
            return genre;
        }

        Movie();

        Movie(string t, unsigned int y, string g) {
            title = t;
            year = y;
            genre = g;

            cout << "Movie object constructed successfully" << endl;
        }

        void print_details() {
            cout << "Movie title: " << title << endl;
            cout << "Year of release: " << year << endl;
            cout << "Release: " << genre << endl;
        }
};

int main(int argc, char **argv) {

    Movie movie("Lagaan", 2003, "Family Movie");
    movie.print_details();


    return 0;
}