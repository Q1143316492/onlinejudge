#ifndef _UNION_TEST_H_
#define _UNION_TEST_H_

#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

void test_ans_compare()
{
    printf("[\\r = %d  \\n = %d]\n", '\r', '\n');

    int ansfd = open("test/data/ans1.txt", O_RDONLY);
    int stdfd = open("test/data/ans2.txt", O_RDONLY);
    char chans = 0, chstd = 0;
    int retans = 0, retstd = 0;
    int flag = 0;   // 0 ac 1 pe 2 wa
    while (true) {
        retans = read(ansfd, &chans, 1);
        retstd = read(stdfd, &chstd, 1);
        if (retans == 0 && retstd == 0) {
            break;
        }
        if (retans && retstd && chans == chstd) {
            continue;
        } else {
            // printf("[%d %d]\n", chans, chstd);
            flag = 2;
            break;
        }
    }
    close(ansfd);
    close(stdfd);

    if (flag == 0) {
        cout << "AC" << endl;
        return ;
    }
    ansfd = open("test/data/ans1.txt", O_RDONLY);
    stdfd = open("test/data/ans2.txt", O_RDONLY);
    flag = 1;
    while (true) {
        while (true) {
            retans = read(ansfd, &chans, 1);
            if (retans == 0 || (chans != ' ' && chans != '\n' && chans != '\t')) {
                break;
            }
        }
        while (true) {
            retstd = read(stdfd, &chstd, 1);
            if (retstd == 0 || (chstd != ' ' && chstd != '\n' && chstd != '\t')) {
                break;
            }
        }
        if (retans == 0 && retstd == 0) break;
        if (retans == 0 || retstd == 0 || chans != chstd) {
            flag = 2;
            break;
        } 
    } 
    if (flag == 1) {
        cout << "PE" << endl;
    } else {
        cout << "WA" << endl;
    }
    close(ansfd);
    close(stdfd);
}

#endif