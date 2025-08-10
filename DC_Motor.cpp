#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include "Eigen/Dense"
#include "seal/seal.h"

using namespace Eigen;
using namespace std;
using namespace seal;

// Utility function to center-align strings in console output
string center(const string &s, int width) {
    int len = s.length();
    int pad = (width - len) / 2;
    if (pad < 0) pad = 0;
    return string(pad, ' ') + s + string(width - pad - len, ' ');
}

int main() {
    //Model parameters
    Matrix2d A;
    A << -10, 1,
         -0.02, -2;
    Vector2d B(0, 2);
    RowVector2d C(1, 0);
    double D = 0;
    double dt = 0.1;
    double u = 100.0;   //fixed input voltage

    cout << "Fixed input voltage used: " << u << " V" << endl;

    //Get user input for desired speed 
    double desired_speed;
    cout << "Enter desired speed (rad/sec): ";
    cin >> desired_speed;

    Vector2d x_normal(0, 0);     //Initial state: speed=0, current=0

    //SEAL CKKS Setup 
    EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = 8192;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(
        poly_modulus_degree, {60, 40, 40, 60}));
    SEALContext context(parms);

    KeyGenerator keygen(context);
    PublicKey public_key;
    keygen.create_public_key(public_key);
    SecretKey secret_key = keygen.secret_key();
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    CKKSEncoder encoder(context);

    double scale = pow(2.0, 40);

    //Initialize encrypted state 
    vector<double> x_vec = {0, 0};
    Plaintext x_plain;
    encoder.encode(x_vec, scale, x_plain);
    Ciphertext x_enc;
    encryptor.encrypt(x_plain, x_enc);

    // Console output header 
    cout << "\n"
         << center("Time(s)", 12)
         << center("Speed_enc", 22)
         << center("Speed_norm", 22)
         << center("Error", 22) << "\n";
    cout << string(78, '-') << "\n";

    //Simulation loop
    double current_time = 0.0;
    double max_sim_time = 100.0;       //safety limit to avoid infinite loops
    cout << fixed << setprecision(6);

    while (true) {
        //Normal simulation (without encryption)
        Vector2d x_next_norm = x_normal + dt * (A * x_normal + B * u);
        double y_normal = (C * x_next_norm)(0, 0);

        //Decrypt and decode encrypted state
        Plaintext x_plain_dec;
        decryptor.decrypt(x_enc, x_plain_dec);
        vector<double> x_dec_vec;
        encoder.decode(x_plain_dec, x_dec_vec);

        //Compute encrypted next state (simulate by decrypt-update-encrypt)
        vector<double> x_new_vec(2);
        x_new_vec[0] = x_dec_vec[0] + dt * (A(0,0) * x_dec_vec[0] + A(0,1) * x_dec_vec[1] + B(0) * u);
        x_new_vec[1] = x_dec_vec[1] + dt * (A(1,0) * x_dec_vec[0] + A(1,1) * x_dec_vec[1] + B(1) * u);

        Plaintext x_new_plain;
        encoder.encode(x_new_vec, scale, x_new_plain);
        encryptor.encrypt(x_new_plain, x_enc);

        double y_enc = x_new_vec[0];
        double err = abs(y_enc - y_normal);

        //Format output
        cout << left << setw(12) << current_time;
        cout << right << setw(22) << setprecision(12) << y_enc;
        cout << setw(22) << y_normal;
        if (err < 1e-6) {
            cout << setw(22) << scientific << setprecision(12) << err << defaultfloat << "\n";
        } else {
            cout << setw(22) << fixed << setprecision(9) << err << "\n";
        }

        //Update time and state
        current_time += dt;
        x_normal = x_next_norm;

        // Stop if desired speed reached (or exceeded) or max time exceeded
        if (y_normal >= desired_speed || current_time >= max_sim_time) {
            break;
        }
    }

    cout << "\nSimulation complete." << endl;
    return 0;
}
