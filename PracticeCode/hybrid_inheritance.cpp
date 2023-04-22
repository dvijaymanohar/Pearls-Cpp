
#include <iostream>
#include <string>

using namespace std;

class Vehicle {
public:
  string reg_code;
  string manufacturer;
  string model;

  Vehicle(string reg_code, string manufacturer, string model) {
    cout << "Vehicle Parameterised constructor called" << endl;
    this->reg_code = reg_code;
    this->manufacturer = manufacturer;
    this->model = model;
  }

  Vehicle() {
    reg_code = "";
    manufacturer = "";
    model = "";
  }

  void getDetails(void) {
    cout << "Registration Code: " << reg_code << endl;
    cout << "Manufacturer: " << manufacturer << endl;
    cout << "Model: " << model << endl;
  }
};

class FuelCar : public Vehicle {
public:
  string fuel_type; /* Gasoline/Diesel */

  FuelCar(string reg_code, string manufacturer, string model, string fuel_type)
      : Vehicle(reg_code, manufacturer, model) {
    cout << "FuelCar parameterised constructor called" << endl;
    this->fuel_type = fuel_type;
  }

  FuelCar() { this->fuel_type = ""; }

  void getDetails(void) { cout << "Fuel Type: " << this->fuel_type << endl; }
};

class ElectricCar : public Vehicle {
public:
  bool li_battery;

  ElectricCar(string reg_code, string manufacturer, string model,
              bool li_battery)
      : Vehicle(reg_code, manufacturer, model) {
    cout << "ElectricCar parameterised constructor called" << endl;
    this->li_battery = li_battery;
  }

  void getDetails(void) {
    cout << "Electric Type: " << this->li_battery << endl;
  }
};

class HybridCar : public FuelCar, public ElectricCar {
public:
  float tank_capacity;
  float battery_capacity;

  HybridCar(string reg_code, string manufacturer, string model,
            string fuel_type, bool li_battery, float tank_capacity,
            float battery_capacity)
      : FuelCar(reg_code, manufacturer, model, fuel_type),
        ElectricCar(reg_code, manufacturer, model, li_battery) {
    cout << "HybridCar parameterised constructor called" << endl;
    this->tank_capacity = tank_capacity;
    this->battery_capacity = battery_capacity;
  }

  void getDetails(void) {

    // cout << "Registration Code: " << this->reg_code << endl;
    // cout << "Manufacturer :" << this->manufacturer << endl;

    cout << "Electric Type: " << this->tank_capacity << endl;
  }
};

int main(int argc, char *argv[]) {
  HybridCar *car =
      new HybridCar("TP650H", "Volswagen", "Up", "Gasolene", true, 40.00, 100);

  car->getDetails();

  delete car;

  return 0;
}