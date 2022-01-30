#include "prompttext.h"


const char* promptText =
R"(Query Tool

This tool only support BBT, LRT, STest and WTest query
query text format as following:

    DEV_D1
    777296
    Apex
    Plugin.Post.Probe.FringeProbeTests.TestPost_WhenClickClearAllButton_ExpectClearAllLabel
    integration\Plugins\PostPlugin\UI\MultiViews\testPlayAnimationForAllViews\testPlayAnimationForAllViews.py <testPlayAnimationForAllViews.test_WhenChangedAnimationRangeAndDuplicate_ExpectAnimationPlaySuccess>
    Plugins\PostPlugin\ContourVisualization\CreateContourVisualization_st.py <CreateContourVisualization_st.test_CreateContourVisualization.test_WhenCreateContourVisualization_ExpectMacroRecordedIdentical>
    Plugin\Post\FreebodyTab\test_ExportDataGrid_wt.py <test_ExportDataGrid_wt.ExportDataGrid.test_ExportDataGrid_ExpectExportDataOK>

the first line means to correspond to <Code Line> from QCRT
the second line is optional that means to correspond to <Change List> from QCRT, if this line does not exist, will query status of top 20 Code Line from QCRT
the third line means to correspond to <APP> from QCRT
next each lines will represent case which can be any one of BBT, LRT, STest and LRT
you can create these text and paste them to above text area and click query button, so the result will write to qcrtReport.txt in current directory and also show them in right area.

)";

const char* querySummaryHead=
R"(The Query summary is as follows:

    Code Line: %1%
    Change List: %2%
    APP: %3%)";

const char* querySummaryCaseHead =
R"(
    The total %1% of %2%:
)";

const char* querySummaryCaseBody =
R"(
    (%1%) %2%
)";
