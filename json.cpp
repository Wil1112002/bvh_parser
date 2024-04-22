#include<iostream>
#include<fstream>
#include"bvh_parser.h"

using std::ofstream;


void jsonify(joint root, META meta_data) {
    ofstream outfile;
    outfile.open("output.json");
    
    // output the root and meta_data


    outfile.close();
}