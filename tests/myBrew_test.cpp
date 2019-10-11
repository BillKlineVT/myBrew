#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <SensorWorkerThread.h>

using namespace testing;

TEST(myBrew_test, sample1)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}

TEST(myBrew_test, sample2)
{
    SensorWorkerThread* swt = new SensorWorkerThread();
    ASSERT_EQ(NULL, swt);
}


//extrapolate_level_value(vector<double> calibration_temps, vector<double> calibration_volumes, vector<vector<int> > calibration_raw_data, double currentTemp, int current_raw_value);
TEST(myBrew_test, extrapolate_level_value)
{
    SensorWorkerThread* swt = new SensorWorkerThread();
    vector<double> caltemps{1,2,3,4,5};
    vector<double> calvols{10,20,30,40};
    vector<int> calrawdata {10021,102010,10001100,1010101};
    double currentTemp = 100.12;
    int currentraw = 10101010;
    swt->extrapolate_level_value(caltemps, calvols, calrawdata, currentTemp, currentraw);
    ASSERT_EQ(100.2,  swt->extrapolate_level_value(caltemps, calvols, calrawdata, currentTemp, currentraw););
}