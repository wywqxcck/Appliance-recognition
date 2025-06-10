#include "KNN.h"


// 排序后的训练数据
float X_train[NUM_SAMPLES][NUM_FEATURES] = { 0 };

// 对应的类别标签
int16_t y_train[NUM_SAMPLES] = { 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5};

// 计算两个点之间的欧式距离
float euclidean_distance(float* a, float* b, int16_t length) {
    float sum = 0.0;
    for (int16_t i = 0; i < length; i++) {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sqrt(sum);
}

// 查找 k 个最近的邻居，并检查距离阈值
int16_t predict(float* test_point, int16_t num_features) {
    float distances[NUM_SAMPLES];
    int16_t neighbors[K_NEIGHBORS];
    // 初始化 k 个最近邻居的索引和距离为最大值
    for (int16_t i = 0; i < K_NEIGHBORS; i++) {
        neighbors[i] = -1;
        distances[i] = INT_MAX;
    }

    // 计算所有训练样本与测试点的距离
    for (int16_t i = 0; i < NUM_SAMPLES; i++) {
        float distance = euclidean_distance(test_point, X_train[i], num_features);

        // 寻找 k 个最近的邻居
        for (int16_t j = 0; j < K_NEIGHBORS; j++) {
            if (distance < distances[j]) {
                // 将当前邻居插入到数组中，更新对应的距离
                for (int16_t l = K_NEIGHBORS - 1; l > j; l--) {
                    distances[l] = distances[l - 1];
                    neighbors[l] = neighbors[l - 1];
                }
                distances[j] = distance;
                neighbors[j] = i;
                break;
            }
        }
    }

    // 检查是否所有最近邻居的距离都超过了阈值
    int16_t all_neighbors_far = 1;
    for (int16_t i = 0; i < K_NEIGHBORS; i++) {
        if (distances[i] <= DISTANCE_THRESHOLD) {
            all_neighbors_far = 0; // 至少有一个邻居在距离阈值范围内
            break;
        }
    }

    // 如果所有邻居都在阈值之外，打印距离并返回异常值标记
    if (all_neighbors_far) {
//        printf("警告: 测试点与训练数据相距太远。所有邻居的距离都超过了阈值。\r\n");
//        printf("超过阈值的距离为:\r\n");
        for (int16_t i = 0; i < K_NEIGHBORS; i++) {
            //printf("邻居 %d 的距离: %.2f\r\n", neighbors[i], distances[i]);
        }
        return -1; // 返回 -1 表示异常
    }
		//printf("邻居 %d 的距离: %.2f\r\n", neighbors[0], distances[0]);
		
    // 统计 k 个邻居中的类别投票
    int16_t class_counts[NUM_CLASS] = { 0 };  // 假设有 3 个类别
    for (int16_t i = 0; i < K_NEIGHBORS; i++) {
        int16_t neighbor_index = neighbors[i];
        if (neighbor_index != -1) {
            class_counts[y_train[neighbor_index]]++;
        }
    }

    // 找到投票最多的类别
    int16_t predicted_class = 0;
    int16_t max_votes = class_counts[0];
    for (int16_t i = 1; i < NUM_CLASS; i++) {
        if (class_counts[i] > max_votes) {
            max_votes = class_counts[i];
            predicted_class = i;
        }
    }
    // 返回最终预测的类别
    return predicted_class;
}

