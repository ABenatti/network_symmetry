#include "CVNetwork.h"
#include "CVConcentricStructure.h"
#include "CVNetworkSymmetry.h"
#include <getopt.h>
#include "CVNetworkCentrality.h"


// #define CVSymmetryApplicationReportParameterError(programName, ...) do {fprintf(stderr,__VA_ARGS__)}


static void CVSymmetryApplicationPrintProgress(CVOperationControl* thisControl);
static void CVSymmetryApplicationPrintCentralityProgress(CVOperationControl* thisControl);
static void CVSymmetryApplicationPrintStressProgress(CVOperationControl* thisControl);
static void CVSymmetryApplicationPrintLiveStream(CVOperationControl* thisControl, CVIndex index, const char* format, ...);
int CVSymmetryApplication(CVNetwork* theNetwork, int argc, char** argv, CVSymmetryOutputParameters** outputArray, CVInteger *level_out);

