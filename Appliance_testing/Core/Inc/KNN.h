#ifndef __KNN_H_
#define __KNN_H_

#include <stdio.h>
#include <math.h>
#include <main.h>

#define NUM_SAMPLES 											18// 样本数量
#define NUM_FEATURES 											3// 特征数量
#define NUM_CLASS													6//类别数量
#define K_NEIGHBORS 											1// k 值 (可以通过此宏修改 k 的值)
#define DISTANCE_THRESHOLD								2.0// 距离阈值
#define INT_MAX 													65535


extern float X_train[NUM_SAMPLES][NUM_FEATURES];
int16_t predict(float* test_point, int16_t num_features);


#endif

