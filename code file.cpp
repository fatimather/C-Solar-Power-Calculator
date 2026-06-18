#include <iostream>
#include <iomanip>
#include <cctype>
#include <string>
#include <cmath>

using namespace std;

/* Creating a function to calculate loss due
to AQI and the dust factor. */
float Environmental_Loss(int aqi, string dustStr) {

    /*Since our dust levels our stored as "high" and "low",
    we'll also use those labels in our functions. */
    float dust_value = 0.0;
    if (dustStr == "Very High") dust_value = 0.20;
    else if (dustStr == "High") dust_value = 0.12;
    else if (dustStr == "Medium") dust_value = 0.05;
    else dust_value = 0.02; // For "Low"

    /* if the AQI is less than 100, the air is pure
    and the power loss will be minimum. We have used 2% loss to
    account for the environmental haze. */
    /* On the other hand, if AQI is more than 100,
    the loss will increase with increase in AQI,
    so we have used the factor aqi * 0.0006. */

    float aqi_factor = (aqi > 100) ? (aqi * 0.0006) : 0.02;
    return aqi_factor + dust_value;
}

float thermal_loss(double temp) {

    /* Voltage is going to drop 0.4% for one degree rise in
    temperature, if the temperature is above 25 degrees Celsius. */

    if (temp > 25)
        return (temp - 25) * 0.004;
    else
        return 0.0; // No loss if temp is 25 or below
}

/* I-V Modeling: Using the Diode equation within a loop to calculate the
current for different voltage points. */
void calculate_IV_Points(double temp, int aqi, string dust) {

    float V_oc = 40.0 * (1 - thermal_loss(temp));
    float I_sc = 10.0 * (1 - Environmental_Loss(aqi, dust));

    const int POINTS = 10;
    const int HEIGHT = 10;

    float voltages[POINTS];
    float currents[POINTS];

    // Generate values
    for (int i = 0; i < POINTS; i++) {
        float v = (V_oc / (POINTS - 1)) * i;
        float current = I_sc * (1 - pow((v / V_oc), 5));

        voltages[i] = v;
        currents[i] = current;
    }

    // Draw graph
    for (int row = HEIGHT; row >= 0; row--) {
        float level = (I_sc / HEIGHT) * row;

        for (int col = 0; col < POINTS; col++) {
            if (currents[col] >= level)
                cout << " * ";
            else
                cout << "   ";
        }
        cout << endl;
    }

    // X-axis
    for (int i = 0; i < POINTS; i++) cout << "---";
    cout << endl;

    // Voltage labels
    for (int i = 0; i < POINTS; i++) {
        cout << setw(3) << fixed << setprecision(0) << voltages[i];
    }
    cout << "V\n";
}

//initializing data structures.
int getCityIndex(string cityName, string cities[], int size) {
    //format input: first letter uppercase, rest lowercase
    cityName[0] = toupper(cityName[0]);
    for (int i = 1; i < cityName.length(); i++) {
        cityName[i] = tolower(cityName[i]);
    }
    //finding city
    for (int i = 0; i < size; i++) {
        if (cities[i] == cityName) {
            return i;
        }
    }
    return -1; // Validation which handles "City Not Found"
}
//Final Solar Audit Report Display Function
void displayReport(string city, double temp, int aqi, float power, float efficiency) {
    cout << "\n===== SOLAR AUDIT REPORT =====\n";
    cout << "City: " << city << endl;
    cout << "Temperature: " << temp << " C" << endl;
    cout << "AQI: " << aqi << endl;
    cout << "-----------------------------\n";
    cout << "Power Output: " << power << " W" << endl;
    cout << "Efficiency: " << efficiency << " %" << endl;
    cout << "=============================\n";
}
int main() {
    const int SIZE = 11;

    //parallel arrays
    string cities[SIZE] = {
        "Lahore", "Faisalabad", "Karachi", "Multan",
        "Jacobabad", "Gujranwala", "Islamabad",
        "Gwadar", "Bahawalpur", "Quetta", "Gilgit"
    };

    double temperatures[SIZE] = {
        24, 23, 27, 26, 29, 24, 21, 26, 27, 18, 12
    };

    int aqi[SIZE] = {
        180, 170, 110, 190, 160, 175, 120, 90, 185, 100, 60
    };

    string dustFactor[SIZE] = {
        "High", "High", "Medium", "Very High", "Very High",
        "High", "Medium", "Low", "Very High", "Medium", "Low"
    };

    string cityname;
    cout << "Please choose any city; Lahore, Faisalabad, Quetta, Karachi, Multan,"<<endl;
    cout<<"Jacobabad, Gujranwala, Islamabad, Bahawalpur, Gilgit : ";
    cin >> cityname;

    int index = getCityIndex(cityname, cities, SIZE);

    if (index != -1) {

        float t_loss = thermal_loss(temperatures[index]);
        float p_loss = Environmental_Loss(aqi[index], dustFactor[index]);

        // Actual Power calculation
        float actualPower = 400.0 * (1 - t_loss) * (1 - p_loss);

        //Calling V-I Graph function
        cout << "\n--- V-I GRAPH ---\n";
        calculate_IV_Points(temperatures[index], aqi[index], dustFactor[index]);
        // Efficiency Calculation against "Model City" (Quetta at index 9)
        const int modelindex = 9;
        float model_t_loss = thermal_loss(temperatures[modelindex]);
        float model_p_loss = Environmental_Loss(aqi[modelindex], dustFactor[modelindex]);
        float modelPower = 400.0 * (1 - model_t_loss) * (1 - model_p_loss);

        cout << " City: " << cities[index] << endl;
        cout << "Power Output: " << actualPower << " W" << endl;
        cout << "Efficiency vs Model City (" << cities[modelindex] << "): "
             << (actualPower / modelPower) * 100 << "%" << endl;
             //Displaying Final Solar Report
             displayReport(cities[index], temperatures[index], aqi[index],
              actualPower, (actualPower / modelPower) * 100);

    } else {
        cout << "City not found." << endl;
    }

    return 0;
}