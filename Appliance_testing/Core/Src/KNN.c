#include "KNN.h"


// ������ѵ������
float X_train[NUM_SAMPLES][NUM_FEATURES] = { 0 };

// ��Ӧ������ǩ
int16_t y_train[NUM_SAMPLES] = { 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5};

// ����������֮���ŷʽ����
float euclidean_distance(float* a, float* b, int16_t length) {
    float sum = 0.0;
    for (int16_t i = 0; i < length; i++) {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sqrt(sum);
}

// ���� k ��������ھӣ�����������ֵ
int16_t predict(float* test_point, int16_t num_features) {
    float distances[NUM_SAMPLES];
    int16_t neighbors[K_NEIGHBORS];
    // ��ʼ�� k ������ھӵ������;���Ϊ���ֵ
    for (int16_t i = 0; i < K_NEIGHBORS; i++) {
        neighbors[i] = -1;
        distances[i] = INT_MAX;
    }

    // ��������ѵ����������Ե�ľ���
    for (int16_t i = 0; i < NUM_SAMPLES; i++) {
        float distance = euclidean_distance(test_point, X_train[i], num_features);

        // Ѱ�� k ��������ھ�
        for (int16_t j = 0; j < K_NEIGHBORS; j++) {
            if (distance < distances[j]) {
                // ����ǰ�ھӲ��뵽�����У����¶�Ӧ�ľ���
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

    // ����Ƿ���������ھӵľ��붼��������ֵ
    int16_t all_neighbors_far = 1;
    for (int16_t i = 0; i < K_NEIGHBORS; i++) {
        if (distances[i] <= DISTANCE_THRESHOLD) {
            all_neighbors_far = 0; // ������һ���ھ��ھ�����ֵ��Χ��
            break;
        }
    }

    // ��������ھӶ�����ֵ֮�⣬��ӡ���벢�����쳣ֵ���
    if (all_neighbors_far) {
//        printf("����: ���Ե���ѵ���������̫Զ�������ھӵľ��붼��������ֵ��\r\n");
//        printf("������ֵ�ľ���Ϊ:\r\n");
        for (int16_t i = 0; i < K_NEIGHBORS; i++) {
            //printf("�ھ� %d �ľ���: %.2f\r\n", neighbors[i], distances[i]);
        }
        return -1; // ���� -1 ��ʾ�쳣
    }
		//printf("�ھ� %d �ľ���: %.2f\r\n", neighbors[0], distances[0]);
		
    // ͳ�� k ���ھ��е����ͶƱ
    int16_t class_counts[NUM_CLASS] = { 0 };  // ������ 3 �����
    for (int16_t i = 0; i < K_NEIGHBORS; i++) {
        int16_t neighbor_index = neighbors[i];
        if (neighbor_index != -1) {
            class_counts[y_train[neighbor_index]]++;
        }
    }

    // �ҵ�ͶƱ�������
    int16_t predicted_class = 0;
    int16_t max_votes = class_counts[0];
    for (int16_t i = 1; i < NUM_CLASS; i++) {
        if (class_counts[i] > max_votes) {
            max_votes = class_counts[i];
            predicted_class = i;
        }
    }
    // ��������Ԥ������
    return predicted_class;
}

