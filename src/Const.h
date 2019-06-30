#pragma once

namespace CONST {
const int MapSize = 25;             // 地图大小

const int MineDistance = 4;       // 中心矿物间距

const float TimeLimit = 1.0;		// 回合时间限制，超时操作会被忽略

const float TotalTimeLimit = 1.5;	// 回合时间限制，超时则会跳过整个回合

const int MinePairNumber = 5;       // 边缘随机生成的矿物对数（中心对称，游戏开始时成对初始化）

const int BaseSize = 3;             // 基地间距

const int MaxSoldierHP = 100;      // 普通兵最大血量

const int MaxArcherHP = 50;         // 弓箭手最大血量

const int SoldierCost = 30;         // 单位造价

const int HP[5] = {0, 100, 100, 100, 50};

const int actPoint[5] = {0, 3, 3, 3, 2};

const int atn[5] = {0, 20, 20, 20, 40};

const int range[5] = {0, 1, 1, 1, 2};
}
