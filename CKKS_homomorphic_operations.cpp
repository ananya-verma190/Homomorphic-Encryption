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
    parms.set_poly_modulus_degree(8192);
    parms.set_coeff_modulus(CoeffModulus::Create(8192, { 60, 40, 40, 60 })); 
    SEALContext context(parms);
    
    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    
    CKKSEncoder encoder(context);
    Encryptor encryptor(context, public_key);
    Decryptor decryptor(context, secret_key);
    Evaluator evaluator(context);
    double scale = pow(2.0, 40);
    
    //first numbers
    vector<double> numbers1;
    string line;
    cout << "Enter first numbers: ";
    getline(cin, line);
    
    string current_number = "";
    for (int j = 0; j < line.length(); j++)
    {
        if (line[j] == ' ')
        {
            if (!current_number.empty())
            {
                numbers1.push_back(stod(current_number));
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
        numbers1.push_back(stod(current_number));
    }
    
    //second numbers
    vector<double> numbers2;
    cout << "Enter second numbers: ";
    getline(cin, line);
    
    current_number = "";
    for (int j = 0; j < line.length(); j++)
    {
        if (line[j] == ' ')
        {
            if (!current_number.empty())
            {
                numbers2.push_back(stod(current_number));
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
        numbers2.push_back(stod(current_number));
    }
    
    int size = min(numbers1.size(), numbers2.size());
    numbers1.resize(size);
    numbers2.resize(size);
    
    //display original numbers
    cout << "\nFirst numbers:  ";
    for (int i = 0; i < size; i++)
    {
        cout << fixed << setprecision(6) << numbers1[i] << " ";
    }
    cout << "\nSecond numbers: ";
    for (int i = 0; i < size; i++)
    {
        cout << fixed << setprecision(6) << numbers2[i] << " ";
    }
    cout << endl;
    
    //encrypt
    Plaintext plain1, plain2;
    encoder.encode(numbers1, scale, plain1);
    encoder.encode(numbers2, scale, plain2);
    Ciphertext encrypted1, encrypted2;
    encryptor.encrypt(plain1, encrypted1);
    encryptor.encrypt(plain2, encrypted2);
    
    //addition
    cout << "\nADDITION:\n";
    Ciphertext result_add;
    evaluator.add(encrypted1, encrypted2, result_add);
    
    Plaintext decrypted_add;
    decryptor.decrypt(result_add, decrypted_add);
    vector<double> homomorphic_add;
    encoder.decode(decrypted_add, homomorphic_add);
    homomorphic_add.resize(size);
    
    cout << "Expected:    ";
    for (int i = 0; i < size; i++)
    {
        cout << fixed << setprecision(10) << numbers1[i] + numbers2[i] << " ";
    }
    cout << "\nHomomorphic: ";
    for (int i = 0; i < size; i++)
    {
        cout << fixed << setprecision(10) << homomorphic_add[i] << " ";
    }
    cout << "\nErrors:      ";
    for (int i = 0; i < size; i++)
    {
        double error = abs((numbers1[i] + numbers2[i]) - homomorphic_add[i]);
        cout << scientific << setprecision(3) << error << "  ";
    }
    cout << endl;
    
    //multiplication
    cout << "\nMULTIPLICATION:\n";
    Ciphertext result_mult;
    evaluator.multiply(encrypted1, encrypted2, result_mult);
    evaluator.relinearize_inplace(result_mult, relin_keys);
    evaluator.rescale_to_next_inplace(result_mult);
    
    Plaintext decrypted_mult;
    decryptor.decrypt(result_mult, decrypted_mult);
    vector<double> homomorphic_mult;
    encoder.decode(decrypted_mult, homomorphic_mult);
    homomorphic_mult.resize(size);
    
    cout << "Expected:    ";
    for (int i = 0; i < size; i++)
    {
        cout << fixed << setprecision(10) << numbers1[i] * numbers2[i] << " ";
    }
    cout << "\nHomomorphic: ";
    for (int i = 0; i < size; i++)
    {
        cout << fixed << setprecision(10) << homomorphic_mult[i] << " ";
    }
    cout << "\nErrors:      ";
    for (int i = 0; i < size; i++)
    {
        double error = abs((numbers1[i] * numbers2[i]) - homomorphic_mult[i]);
        cout << scientific << setprecision(3) << error << "  ";
    }
    cout << endl;
    
    return 0;
}
