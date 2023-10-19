#include <gtest/gtest.h>
#include <stdlib.h>

extern "C" {
#include "../../src/custom_data_types/list.c"
}


TEST(CustomDataTypes, Test1) {
    list_t my_list = list_init(0);

    int x = 69;
    int y = 420;

    list_push(my_list,(void *)&x);

    EXPECT_EQ(list_length(my_list), 1);

    list_push(my_list,(void *)&y);

    EXPECT_EQ(list_length(my_list), 2);
    
    list_pop(my_list);

    EXPECT_EQ(list_length(my_list), 1);
}