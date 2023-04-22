#include <iostream>
#include <string>

using namespace std;

class Vehicle {
    public:
        string manufacturer;
        string model_name;

        Vehicle(string manufacturer, string model_name) {
            this->manufacturer = manufacturer;
            this->model_name = model_name;
        }

        Vehicle() {
            manufacturer = "";
            model_name = "";
        }

        string getModelName() {
            return model_name;
        }

        string getManufacture() {
            return manufacturer;
        }
};

class Documentation {
    public:
        string document_name;

        string get_document_name() {
            return document_name;
        }

        Documentation(string doc_name) {
            this->document_name = doc_name;
        }
};

class ElectricVehicle : public Vehicle, public Documentation {
    public:
        float mileage;

        ElectricVehicle(string manufacturer, string model_name, string document_name, float mileage): Vehicle(manufacturer, model_name),
            Documentation(document_name) {
            this->mileage = mileage;
        }

        float getMilage() {
            return this->mileage;
        }
};

int main(int argc, char** argv) {
    ElectricVehicle evehicle("Tesla", "Latest", "RC Card", 600.00);

    cout << "Electrical vehicle model name: " << evehicle.getModelName() << endl;
    cout << "Vehicle mileage: " << evehicle.getMilage() << endl;

    return 0;
}