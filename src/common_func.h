#ifndef _COMMON_FUNC_H_
#define _COMMON_FUNC_H_

#include <iostream>
#include <utility>
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <arpa/inet.h>
#include <cstring>
#include <random>
#include <stdexcept>
#include <sstream>
#include <string>

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::string;
using std::unordered_map;
using std::vector;

using namespace std;

int prime_seeds[] = {37, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, \
                      103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, \
                      163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, \
                      227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, \
                      281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, \
                      353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, \
                      421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, \
                      487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, \
                      569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, \
                      631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, \
                      701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, \
                      773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, \
                      857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, \
                      937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, \
                      1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, \
                      1063, 1069, 1087, 1091, 1093, 1097, 1103, 1109, 1117, \
                      1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, \
                      1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, \
                      1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, \
                      1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, \
                      1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, \
                      1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, \
                      1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, \
                      1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637,};

/************************** Loading Traces ******************************/

#define NUM_TRACE 12               // Number of traces in DATA directory
#define TIMES 10                   // Times of each algorithm measuring the same
                                   // trace using different hash function
#define DATA_ROOT_15s "/data/data" // NUM_TRACE = 1, CAIDA
#define MY_RANDOM_SEED 813

struct SRCIP_TUPLE
{
  char key[13] = {0};
};
struct REST_TUPLE
{
  char key[9];
};

typedef vector<SRCIP_TUPLE> TRACE;

TRACE traces[NUM_TRACE];

/************/

string Stringsplit(string str)
{
	istringstream iss(str);
	string token;
  int num = 0;
	while (getline(iss, token, '|'))
	{
        if (num == 0)
            break;
        num++;
	}
    return token;
}

void tochar(string s, char arr[13]) {
    int num = stoi(s);
    std::stringstream ss;  
    ss << std::hex << num;
    std::string hexStr = ss.str();
    if (hexStr.length() > 4) {  
        hexStr = hexStr.substr(hexStr.length() - 4); // 取最后4个字符  
    } 
    for (size_t i = 0; i < hexStr.length(); i++) {  
        arr[i] = hexStr[i];
    }
}

uint32_t readData()
{
  uint32_t total_pck_num = 0;
  string filenames[3] = {
    "/home/lzx/ERSketch-final/Tasks/Heavy-hitter_detection_and_others/tpc-ds/store_sales.dat",
    "/home/lzx/ERSketch-final/Tasks/Heavy-hitter_detection_and_others/tpc-ds/web_sales.dat",
    "/home/lzx/ERSketch-final/Tasks/Heavy-hitter_detection_and_others/tpc-ds/catalog_sales.dat"
  };
  int window = 0;
  string line;
  SRCIP_TUPLE key;

  for (const string& filename : filenames) {
  ifstream file(filename.c_str());
  if (!getline(file, line)){
    printf("[ERROR] file error!\n");
    exit(0);
  }
  while (getline(file, line)) {
    string token = Stringsplit(line);
    if (token == "")
      continue; 
    // cout << "token = " << token << endl; 
    // break;
    char tmp[13] = {0};
    tochar(token.c_str(), tmp);
    memcpy(&key, tmp, 4);
    traces[window].push_back(key);
    total_pck_num++;
  }
  // printf("[INFO] Scanned, packets number:\n");
  // int i = 0;
  // printf("[INFO] window %02d has %ld packets\n", i, traces[i].size());
  }
  printf("[INFO] Scanned, packets number:\n");
  int i = 0;
  printf("[INFO] window %02d has %ld packets\n", i, traces[i].size());
  return total_pck_num;
}

/************/

uint32_t ReadTwoWindows()
{
  TRACE all_packets;
  uint32_t total_pck_num = 0;
  string filename130000 = "/root/ERSketch1/data/0.dat";
  char tmp[13] = {0};
  FILE *file1 = fopen(filename130000.c_str(), "r");
  if (file1 == NULL)
  {
    printf("[ERROR] file not open\n");
    exit(0);
  }
  SRCIP_TUPLE key;
  int window = 0;
  if (!fread(tmp, 13, 1, file1)){
    printf("[ERROR] file error!\n");
    exit(0);
  }
  while (fread(tmp, 13, 1, file1))
  {
    memcpy(&key, tmp, 4);
    all_packets.push_back(key);
    total_pck_num++;
  }
  for(int i = 0; i < total_pck_num / 2; i++){
    key = all_packets[i];
    traces[0].push_back(key);
  }
  for(int i = total_pck_num / 2; i < total_pck_num; i++){
    key = all_packets[i];
    traces[1].push_back(key);
  }
  printf("[INFO] Scanned, packets number:\n");
  fclose(file1);
  printf("[INFO] window %02d has %ld packets\n", 0, traces[0].size());
  printf("[INFO] window %02d has %ld packets\n", 1, traces[1].size());
  return total_pck_num;
}

uint32_t ReadNTraces(int n)
{
    if (n < 0 || n > 10) {
        printf("[ERROR] Invalid value of n. It should be between 0 and 10.\n");
        return 0;
    }

    uint32_t total_pck_num = 0;
    char tmp[13] = {0};

    for (int i = 0; i <= n; i++) {
        string filename = "/home/lzx/ERSketch01/data/" + to_string(i) + ".dat";
        FILE *file = fopen(filename.c_str(), "r");
        if (file == NULL) {
            printf("[ERROR] File %s not open\n", filename.c_str());
            continue;
        }

        SRCIP_TUPLE key;
        int window = 0;

        if (!fread(tmp, 13, 1, file)) {
            printf("[ERROR] File %s error!\n", filename.c_str());
            fclose(file);
            continue;
        }

        while (fread(tmp, 13, 1, file)) {
            memcpy(&key, tmp, 4);
            traces[window].push_back(key);
            total_pck_num++;
        }

        fclose(file);
        printf("[INFO] File %s scanned, packets number: %ld\n", filename.c_str(), traces[window].size());
    }

    printf("[INFO] Total packets number: %u\n", total_pck_num);
    return total_pck_num;
}

uint32_t myReadTraces()
{
  uint32_t total_pck_num = 0;
  string filename130000 = "/root/ERSketch1/data/0.dat";
  //string filename130000 = "/share/jjn/datasets/MAWI2000000.dat";
  char tmp[13] = {0};
  FILE *file1 = fopen(filename130000.c_str(), "r");
  if (file1 == NULL)
  {
    printf("[ERROR] file not open\n");
    exit(0);
  }
  SRCIP_TUPLE key;
  int window = 0;
  if (!fread(tmp, 13, 1, file1)){
    printf("[ERROR] file error!\n");
    exit(0);
  }
  while (fread(tmp, 13, 1, file1))
  {
    memcpy(&key, tmp, 4);
    traces[window].push_back(key);
    total_pck_num++;
  }
  printf("[INFO] Scanned, packets number:\n");
  fclose(file1);
  int i = 0;
  printf("[INFO] window %02d has %ld packets\n", i, traces[i].size());
  return total_pck_num;
}

uint32_t ReadTraces()
{
  double starttime, nowtime;
  uint32_t total_pck_num = 0;
  string filename130000 = "/home/lzx/ERSketch01/data/0.dat";
  // string filename130000 = "../data/130000.dat";
  char tmp[21] = {0};
  FILE *file1 = fopen(filename130000.c_str(), "r");
  if (file1 == NULL)
  {
    printf("[ERROR] file not open\n");
    exit(0);
  }
  SRCIP_TUPLE key;
  int window = 0;
  if (!fread(tmp, 21, 1, file1)){
    printf("[ERROR] file error!\n");
    exit(0);
  }
  starttime = *(double *)(tmp + 13);
  while (fread(tmp, 21, 1, file1))
  {
    nowtime = *(double *)(tmp + 13);
    if (nowtime - starttime >= 5.0)
    {
      window++;
      starttime = nowtime;
    }
    memcpy(&key, tmp, 4);
    traces[window].push_back(key);
    total_pck_num++;
  }
  printf("[INFO] 12 windows scanned, packets number:\n");
  for (int i = 0; i < 12; i++)
    printf("[INFO] window %02d has %ld packets\n", i, traces[i].size());
  printf("\n\n");
  return total_pck_num;
}
/************************** PREDEFINED NUMBERS***********************/
#define HH_THRESHOLD 500 // 20,000,000 * 0.0005 (0.05%)
#define HC_THRESHOLD 250
#define TOT_MEM 1000
/************************** COMMON FUNCTIONS*************************/
#define ROUND_2_INT(f) ((int)(f >= 0.0 ? (f + 0.5) : (f - 0.5)))

/********************************************************************/

// Count-Min
#define INIT ((uint32_t)random() % 800)
#define CM_BYTES TOT_MEM * 1024 
#define CM_DEPTH 3       // depth of CM
/********************************************************************/
#endif
