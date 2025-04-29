#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include "omp_apps.h"

// 定义测试的调度策略数量
#define NUM_SCHEDULES 5
// 定义每个调度策略测试的不同块大小
#define NUM_CHUNK_SIZES 5
// 定义每种配置的重复测试次数
#define NUM_REPEATS 3

// 用于存储原始图像处理函数的备份
char* (*original_image_proc)(const char*) = NULL;

// 使用静态调度的图像处理函数
char* image_proc_static(const char* filename, int chunk_size) {
   bmp_img img, img_copy;
   if (bmp_img_read(&img, filename) != 0)
      return 0;

   char *res = (char*)calloc(32, sizeof(char));
   strncat(res, filename, strlen(filename) - 4);
   strcat(res, "_sobel_static.bmp");

   bmp_img_read(&img_copy, filename);

   unsigned int wid = img.img_header.biWidth;
   unsigned int hgt = img.img_header.biHeight;
   bmp_img_init_df(&img_copy, wid, hgt);

   // 使用静态调度
   if (chunk_size > 0) {
      #pragma omp parallel for collapse(2) schedule(static, chunk_size)
      for (int i = 1; i < hgt-1; i++) {
         for (int j = 1; j < wid-1; j++) {
            sobel_filter(img.img_pixels, img_copy.img_pixels, i, j);
         }
      }
   } else {
      #pragma omp parallel for collapse(2) schedule(static)
      for (int i = 1; i < hgt-1; i++) {
         for (int j = 1; j < wid-1; j++) {
            sobel_filter(img.img_pixels, img_copy.img_pixels, i, j);
         }
      }
   }
   
   bmp_img_write(&img_copy, res);
   bmp_img_free(&img_copy);
   bmp_img_free(&img);
   return res;
}

// 使用动态调度的图像处理函数
char* image_proc_dynamic(const char* filename, int chunk_size) {
   bmp_img img, img_copy;
   if (bmp_img_read(&img, filename) != 0)
      return 0;

   char *res = (char*)calloc(32, sizeof(char));
   strncat(res, filename, strlen(filename) - 4);
   strcat(res, "_sobel_dynamic.bmp");

   bmp_img_read(&img_copy, filename);

   unsigned int wid = img.img_header.biWidth;
   unsigned int hgt = img.img_header.biHeight;
   bmp_img_init_df(&img_copy, wid, hgt);

   // 使用动态调度
   #pragma omp parallel for collapse(2) schedule(dynamic, chunk_size)
   for (int i = 1; i < hgt-1; i++) {
      for (int j = 1; j < wid-1; j++) {
         sobel_filter(img.img_pixels, img_copy.img_pixels, i, j);
      }
   }
   
   bmp_img_write(&img_copy, res);
   bmp_img_free(&img_copy);
   bmp_img_free(&img);
   return res;
}

// 使用引导式调度的图像处理函数
char* image_proc_guided(const char* filename, int chunk_size) {
   bmp_img img, img_copy;
   if (bmp_img_read(&img, filename) != 0)
      return 0;

   char *res = (char*)calloc(32, sizeof(char));
   strncat(res, filename, strlen(filename) - 4);
   strcat(res, "_sobel_guided.bmp");

   bmp_img_read(&img_copy, filename);

   unsigned int wid = img.img_header.biWidth;
   unsigned int hgt = img.img_header.biHeight;
   bmp_img_init_df(&img_copy, wid, hgt);

   // 使用引导式调度
   if (chunk_size > 0) {
      #pragma omp parallel for collapse(2) schedule(guided, chunk_size)
      for (int i = 1; i < hgt-1; i++) {
         for (int j = 1; j < wid-1; j++) {
            sobel_filter(img.img_pixels, img_copy.img_pixels, i, j);
         }
      }
   } else {
      #pragma omp parallel for collapse(2) schedule(guided)
      for (int i = 1; i < hgt-1; i++) {
         for (int j = 1; j < wid-1; j++) {
            sobel_filter(img.img_pixels, img_copy.img_pixels, i, j);
         }
      }
   }
   
   bmp_img_write(&img_copy, res);
   bmp_img_free(&img_copy);
   bmp_img_free(&img);
   return res;
}

// 使用自动调度的图像处理函数
char* image_proc_auto(const char* filename) {
   bmp_img img, img_copy;
   if (bmp_img_read(&img, filename) != 0)
      return 0;

   char *res = (char*)calloc(32, sizeof(char));
   strncat(res, filename, strlen(filename) - 4);
   strcat(res, "_sobel_auto.bmp");

   bmp_img_read(&img_copy, filename);

   unsigned int wid = img.img_header.biWidth;
   unsigned int hgt = img.img_header.biHeight;
   bmp_img_init_df(&img_copy, wid, hgt);

   // 使用自动调度
   #pragma omp parallel for collapse(2) schedule(auto)
   for (int i = 1; i < hgt-1; i++) {
      for (int j = 1; j < wid-1; j++) {
         sobel_filter(img.img_pixels, img_copy.img_pixels, i, j);
      }
   }
   
   bmp_img_write(&img_copy, res);
   bmp_img_free(&img_copy);
   bmp_img_free(&img);
   return res;
}

// 执行性能测试
void run_performance_test(const char* filename) {
    int max_threads = omp_get_max_threads();
    int thread_counts[] = {1, 2, 4, max_threads};
    int num_threads = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    // 不同的块大小
    int chunk_sizes[] = {1, 4, 16, 64, 256};
    
    // 为结果准备输出文件
    FILE *output = fopen("sobel_performance_results.csv", "w");
    if (!output) {
        perror("无法创建输出文件");
        return;
    }
    
    // 写入CSV表头
    fprintf(output, "策略,线程数,块大小,运行次数,执行时间(秒)\n");
    
    double times[NUM_SCHEDULES][num_threads][NUM_CHUNK_SIZES][NUM_REPEATS];
    char *result_file;
    
    printf("开始性能测试，请等待...\n");
    
    // 测试不同的线程数
    for (int t = 0; t < num_threads; t++) {
        int thread_count = thread_counts[t];
        printf("测试 %d 个线程...\n", thread_count);
        omp_set_num_threads(thread_count);
        
        // 测试静态调度
        for (int c = 0; c < NUM_CHUNK_SIZES; c++) {
            int chunk = chunk_sizes[c];
            printf("  静态调度，块大小: %d\n", chunk);
            
            for (int r = 0; r < NUM_REPEATS; r++) {
                double start_time = omp_get_wtime();
                result_file = image_proc_static(filename, chunk);
                double end_time = omp_get_wtime();
                times[0][t][c][r] = end_time - start_time;
                
                fprintf(output, "静态,%d,%d,%d,%f\n", 
                        thread_count, chunk, r+1, times[0][t][c][r]);
                
                if (result_file) free(result_file);
            }
        }
        
        // 测试动态调度
        for (int c = 0; c < NUM_CHUNK_SIZES; c++) {
            int chunk = chunk_sizes[c];
            printf("  动态调度，块大小: %d\n", chunk);
            
            for (int r = 0; r < NUM_REPEATS; r++) {
                double start_time = omp_get_wtime();
                result_file = image_proc_dynamic(filename, chunk);
                double end_time = omp_get_wtime();
                times[1][t][c][r] = end_time - start_time;
                
                fprintf(output, "动态,%d,%d,%d,%f\n", 
                        thread_count, chunk, r+1, times[1][t][c][r]);
                
                if (result_file) free(result_file);
            }
        }
        
        // 测试引导式调度
        for (int c = 0; c < NUM_CHUNK_SIZES; c++) {
            int chunk = chunk_sizes[c];
            printf("  引导式调度，块大小: %d\n", chunk);
            
            for (int r = 0; r < NUM_REPEATS; r++) {
                double start_time = omp_get_wtime();
                result_file = image_proc_guided(filename, chunk);
                double end_time = omp_get_wtime();
                times[2][t][c][r] = end_time - start_time;
                
                fprintf(output, "引导式,%d,%d,%d,%f\n", 
                        thread_count, chunk, r+1, times[2][t][c][r]);
                
                if (result_file) free(result_file);
            }
        }
        
        // 测试自动调度
        printf("  自动调度\n");
        for (int r = 0; r < NUM_REPEATS; r++) {
            double start_time = omp_get_wtime();
            result_file = image_proc_auto(filename);
            double end_time = omp_get_wtime();
            times[3][t][0][r] = end_time - start_time;
            
            fprintf(output, "自动,%d,N/A,%d,%f\n", 
                    thread_count, r+1, times[3][t][0][r]);
            
            if (result_file) free(result_file);
        }
    }
    
    fclose(output);
    printf("性能测试完成！结果已保存到 sobel_performance_results.csv\n");
    
    // 输出性能摘要
    printf("\n性能摘要（平均执行时间）:\n");
    printf("------------------------------------------------------\n");
    printf("策略\t线程数\t块大小\t平均时间(秒)\n");
    printf("------------------------------------------------------\n");
    
    // 计算并显示平均时间
    for (int s = 0; s < 3; s++) {  // 静态、动态、引导式
        const char* sched_name = (s == 0) ? "静态" : ((s == 1) ? "动态" : "引导式");
        
        for (int t = 0; t < num_threads; t++) {
            int thread_count = thread_counts[t];
            
            for (int c = 0; c < NUM_CHUNK_SIZES; c++) {
                int chunk = chunk_sizes[c];
                double avg_time = 0.0;
                
                for (int r = 0; r < NUM_REPEATS; r++) {
                    avg_time += times[s][t][c][r];
                }
                avg_time /= NUM_REPEATS;
                
                printf("%s\t%d\t%d\t%f\n", sched_name, thread_count, chunk, avg_time);
            }
        }
    }
    
    // 自动调度
    for (int t = 0; t < num_threads; t++) {
        int thread_count = thread_counts[t];
        double avg_time = 0.0;
        
        for (int r = 0; r < NUM_REPEATS; r++) {
            avg_time += times[3][t][0][r];
        }
        avg_time /= NUM_REPEATS;
        
        printf("自动\t%d\tN/A\t%f\n", thread_count, avg_time);
    }
    
    printf("------------------------------------------------------\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("用法: %s <bmp文件路径>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    
    // 运行性能测试
    run_performance_test(filename);
    
    return 0;
}