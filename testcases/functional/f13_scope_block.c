int main()
{
    int x = 521;
    int y = 1;
    {
        int y = 142;
        x = x + y;
    }
    return y;
}
