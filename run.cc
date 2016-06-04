// This is a test function to generate some assembly with jumps

int run (int i) {
    int iteration;
    while (i > 0) {
        if (i % 2)
            i = 3 * i + 1;
        else
            i /= 2;
        if (i % 64)
            break;
        ++iteration;
    }
    return iteration;
}

