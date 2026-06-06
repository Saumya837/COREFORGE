#pragma once

struct ControlBlock{
    int ref_count; //to keep track of how many refrences are there to the same pointer
    int weak_count;

    ControlBlock(): ref_count(1), weak_count(0) {};
};
