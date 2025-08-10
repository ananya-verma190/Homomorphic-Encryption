#include "seal/seal.h"
#include <iomanip>
#include <iostream>
#include <vector>
using namespace std;
using namespace seal;

int main()
{
    //setup encryption stuff
    EncryptionParameters parms(scheme_type::ckks); 
    parms.set_poly_modulus_degree(8192); //N
    parms.set_coeff_modulus(CoeffModulus::Create(8192, { 60, 40, 40, 60 })); 

    SEALContext context(parms);
    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);

    CKKSEncoder encoder(context);
    Encryptor encryptor(context, public_key);
    Decryptor decryptor(context, secret_key);

    double scale = pow(2.0, 40);

    //input numbers
    vector<double> numbers;
    string line;

    cout << "Enter data: ";
    getline(cin, line);

    //parsing
    string current_number = "";
    for (int j = 0; j < line.length(); j++)
    {
        if (line[j] == ' ')
        {
            if (!current_number.empty())
            {
                numbers.push_back(stod(current_number));
                current_number = "";
            }
        }
        else
        {
            current_number += line[j];
        }
    }

    if (!current_number.empty())
    {
        numbers.push_back(stod(current_number));
    }

    //display original numbers
    cout << "\nOriginal numbers: ";
    int i;
    for (i = 0; i < numbers.size(); i++)
    {
        cout << fixed << setprecision(10) << numbers[i] << " ";
    }
    cout << endl;

    //encrypt
    Plaintext plain;
    encoder.encode(numbers, scale, plain);

    Ciphertext encrypted;
    encryptor.encrypt(plain, encrypted);

    //decrypt
    Plaintext decrypted_plain;
    decryptor.decrypt(encrypted, decrypted_plain);

    vector<double> decrypted_numbers;
    encoder.decode(decrypted_plain, decrypted_numbers);
    decrypted_numbers.resize(numbers.size());

    //display decrypted numbers
    cout << "Decrypted numbers: ";
    for (i = 0; i < decrypted_numbers.size(); i++)
    {
        cout << fixed << setprecision(10) << decrypted_numbers[i] << " ";
    }
    cout << endl;

    //calculate errors
    cout << "\nErrors: ";
    for (i = 0; i < numbers.size(); i++)
    {
        double error = abs(numbers[i] - decrypted_numbers[i]);
        cout << scientific << setprecision(6) << error << "    ";
    }
    cout << endl;

    return 0;
}
