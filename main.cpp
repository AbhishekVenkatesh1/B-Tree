//
//  main.cpp
//  HW-7
//
//  Created by Abhishek Venkatesh on 5/19/18.
//  Copyright © 2018 Abhishek Venkatesh. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cmath>
#include "btree.h"


int main(int argc, const char * argv[]) {
    BTree<int, 5> tree;
     
    std::ifstream in("/Users/abhishek.venkatesh/Desktop/CMSC 270/HW-7/HW-7/insert.txt");
    std::ifstream in1("/Users/abhishek.venkatesh/Desktop/CMSC 270/HW-7/HW-7/delete.txt");
    std::ofstream out("/Users/abhishek.venkatesh/Desktop/CMSC 270/HW-7/HW-7/output.txt");
    std::ofstream out1("/Users/abhishek.venkatesh/Desktop/CMSC 270/HW-7/HW-7/output1.txt");
    
    int x;
    while(in >> x) {
        tree.insert(x);
        tree.snapShot(out);
    }
    
    tree.traversal();
    
    std::cout << std::endl;
    std::cout << std::endl;
    
    int y;
    while(in1 >> y) {
        tree.BTreeDelete(y);
        tree.snapShot(out1);
    }
    
    tree.traversal();
    
    
    return 0;
}
