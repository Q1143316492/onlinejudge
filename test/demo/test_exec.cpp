#include <bits/stdc++.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

using namespace std;
const int MAXN = 5e7 + 10;

int main()
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    printf("socket = %d\n", socket);
    return 0;
}