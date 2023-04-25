kernel void Start(global const int* input, global unsigned short* output)
{
    const size_t gID = get_global_id(0);

    unsigned short mask = 0x10000 >> input[gID];
    if (!mask)
        mask = 0xFF80;
        
    output[gID] = mask;
}

kernel void Map(global const int* input, 
    global unsigned short* output, 
    local unsigned short* aux)
{
    const size_t gID = get_group_id(0);

    const size_t gX = gID / 9;
    const size_t gY = gID - (gX * 9);

    const size_t lX = (gX / 3) * 3;
    const size_t lY = (gY / 3) * 3;
    const size_t lID = get_local_id(1);

    const bool sameGrid = lID < 4;
    const bool useX = (lID - 4) > 7;

    size_t x = ((gX - lX) + (((lID & 0x1) + 1) * sameGrid)) % 3;
    size_t y = ((gY - lY) + (((lID >> 1) + 1) * sameGrid)) % 3;

    const size_t rcOffset = (((lID - 4) & 0x7) + 1) * (!sameGrid);

    x = (lX + x + (rcOffset * useX)) % 9;
    y = (lY + y + (rcOffset * (!useX))) % 9;

    aux[lID] = ~(0x10000 >> input[(x * 9) + y]);

    for (size_t n = get_local_size(1) >> 1; n > 0; n >>= 1)
    {
        barrier(CLK_LOCAL_MEM_FENCE);

        if (lID < n)
            aux[lID] &= aux[lID + n];

        if ((n & 0x1) && n > 1)
            n++;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    if (lID == 0)
        output[(gX * 9) + gY] &= aux[0];
}

kernel void CollideCell(
    global const unsigned short* input, 
    global unsigned short* output,
    local unsigned short* aux)
{
    const size_t gID = get_group_id(0);

    const size_t gX = gID / 9;
    const size_t gY = gID - (gX * 9);

    const size_t lX = (gX / 3) * 3;
    const size_t lY = (gY / 3) * 3;
    const size_t lID = get_local_id(1);

    const size_t tX = lID / 8;
    const size_t tY = lID - (tX * 8);

    size_t x = lX + ((gX - lX + tX) % 3);
    size_t y = lY + ((gY - lY + tY) % 3);

    aux[lID] = input[(gX * 9) + gY] & (~input[(x * 9) + y]);

    for (size_t n = get_local_size(1) >> 1; n > 0; n >>= 1)
    {
        barrier(CLK_LOCAL_MEM_FENCE);

        if (lID < n)
            aux[lID] &= aux[lID + n];

        if ((n & 0x1) && n > 1)
            n++;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    if (lID == 0)
        output[(gX * 9) + gY] = aux[0];
}

kernel void CollideRow(
    global const unsigned short* input, 
    global unsigned short* output,
    local unsigned short* aux
)
{
    const size_t gID = get_group_id(0);

    const size_t gX = gID / 9;
    const size_t gY = gID - (gX * 9);

    const size_t lID = get_local_id(1);

    size_t x = (gX + lID + 1) % 9;

    aux[lID] = ~input[(x * 9) + gY];

    for (size_t n = get_local_size(1) >> 1; n > 0; n >>= 1)
    {
        barrier(CLK_LOCAL_MEM_FENCE);

        if (lID < n)
            aux[lID] &= aux[lID + n];

        if ((n & 0x1) && n > 1)
            n++;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    if (lID == 0)
        output[(gX * 9) + gY] |= input[(gX * 9) + gY] & aux[0];
}

kernel void CollideCol(
    global const unsigned short* input, 
    global unsigned short* output,
    local unsigned short* aux
)
{
    const size_t gID = get_group_id(0);

    const size_t gX = gID / 9;
    const size_t gY = gID - (gX * 9);

    const size_t lID = get_local_id(1);

    size_t y = (gY + lID + 1) % 9;

    aux[lID] = ~input[(gX * 9) + y];

    for (size_t n = get_local_size(1) >> 1; n > 0; n >>= 1)
    {
        barrier(CLK_LOCAL_MEM_FENCE);

        if (lID < n)
            aux[lID] &= aux[lID + n];

        if ((n & 0x1) && n > 1)
            n++;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    if (lID == 0)
        output[(gX * 9) + gY] |= input[(gX * 9) + gY] & aux[0];
}

kernel void WriteBack(global const unsigned short* input, global int* output)
{
    const size_t gID = get_global_id(0);

    unsigned short data = input[gID] >> 6;
    int sum = 0;

    for (size_t i = 9; i > 0; i--)
    {
        data >>= 1;
        sum += (data & 0x1) * i;
    }

    output[gID] = sum;
}