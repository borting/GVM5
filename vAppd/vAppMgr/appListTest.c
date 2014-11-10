#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "appList.h"

int main(int argc, char *argv[], char *env[])
{
    int retval;
    char *appListFile = "List";
    unsigned short pvt_port;
    unsigned short state;

    newAppList(appListFile);

    pvt_port = 10001;
    addApp(123, &pvt_port, appListFile);
    pvt_port = 10002;
    addApp(12, &pvt_port, appListFile);
    pvt_port = 10234;
    addApp(1234, &pvt_port, appListFile);
    pvt_port = 10123;
    addApp(4123, &pvt_port, appListFile);

    retval = searchApp(123, &pvt_port, appListFile);
    printf("search pid 123 = %d %hu\n", retval, pvt_port);
    
    retval = searchApp(234, &pvt_port, appListFile);
    printf("search pid 234 = %d %hu\n", retval, pvt_port);

    pvt_port = 10123;
    retval = getAppPid(&pvt_port, appListFile);
    printf("pid %d has port %hu\n", retval, pvt_port);

    pvt_port = 10135;
    retval = getAppPid(&pvt_port, appListFile);
    printf("pid %d has port %hu\n", retval, pvt_port);

    state = 1;
    retval = updateApp(123, &pvt_port, &state, appListFile);
    printf("update pid 123 = %d %hu\n", retval, pvt_port);

    retval = updateApp(1234, &pvt_port, &state, appListFile);
    printf("update pid 1234 = %d %hu\n", retval, pvt_port);

    retval = updateApp(234, &pvt_port, &state, appListFile);
    printf("update pid 234 = %d %hu\n", retval, pvt_port);

    retval = delApp(123, &pvt_port, &state, appListFile);
    printf("delete pid 123 = %d %hu %hu\n", retval, pvt_port, state);

    retval = delApp(123, &pvt_port, &state, appListFile);
    printf("delete pid 123 = %d %hu %hu\n", retval, pvt_port, state);

    //delAppList(appListFile);

    return 0;
}
