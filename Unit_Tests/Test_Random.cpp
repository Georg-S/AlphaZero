#include <gtest/gtest.h>
#include <stddef.h>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <random>
#include <ratio>
#include <omp.h>

#include <vector>


TEST(RandomTest, parallel) {
    #pragma omp parallel for
    for(int i=0;i<10;i++){
        std::cout << i << std::endl;
    }
}