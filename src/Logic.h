#pragma once
#include "Record.h"
#include "Const.h"
#include "ctime"
#include "jsoncpp/json/json.h"
using namespace std;
using namespace CONST;

class state
{
public:
    int mine;//是否有法阵和法阵的等级：0表示随机刷新的周边法阵，1表示中心的三个
    int ownerID;//所有者：0 或者 1
    int working; //表示占领的回合数，working >= 2表示这个回合已经开始产生能量
    
    state(int _mine, int _ownerID, int _working) {
        mine = _mine;
        ownerID = _ownerID;
        working = _working;
    }
    
    state(const state& other) {
        mine = other.mine;
        ownerID = other.ownerID;
        working = other.working;
    }
    
    state(){
        mine = -1;
        ownerID = -1;
        working = 0;
    }
    
    void reset() {
        mine = -1;
        ownerID = -1;
        working = 0;
    }
};

class Soldier {
public:
    bool fake, hide; // fake表示幻像单位，hide表示潜行单位（看不到的地方潜行单位不会显示）
    int actPoint; // 单位余下的行动点数（敌对单位行动点数为0表示已经反击）
    int maxActPoint; // 单位的最大行动点数（如弓箭手是2，幻像是4）
    int kind; // 1 2 3 对应火、冰、风（1 > 2 > 3 > 1)，4 对应弓箭手
    int atn; // 攻击力，近战是20，远程是40。攻击结算是再计算实际伤害
    int HP; // 剩余血量
    int maxHP; // 最大血量，近战是100，远程是50
    int range; // 攻击距离，近战是1，远程是2
    int ownerID; // 单位的归属， ownerID == Logic::Instance()->GetId() 表示是自己的单位。 （ownerID == -1 表示空地）
    int cost; // 单位造价，普通单位是30，幻像是10
    Soldier() {
        ownerID = -1;
        fake = false;
        hide = false;
    }
};

class Logic {
private: // 请不要试图访问或修改 private 变量，这是违反规则的，且可能导致不可预知的错误。
    Soldier map[MapSize][MapSize];
    state mine[MapSize][MapSize];
    int HP[2], money;
    Record* record;
    bool builtIn0, builtIn1, builtIn2;
    static Logic* instance;
    int id;
    Logic();
    Logic(Logic const&);
    bool InMap(int x, int y);
    clock_t startTime;
    int turn;
    
public:
    static Logic* Instance() {
        if (Logic::instance == NULL) {
            Logic::instance = new Logic();
        }
        return Logic::instance;
    }
    
    void Reset();
    
    void Init(char *mapinfo, Record* record);
    
    // 仅使用以下函数
    float UsedTime();
    // 返回这个回合已经消耗的时间
    // 从开始调用你的AI函数开始计时
    
    bool TimeLimitExceeded();
    // 如果已经超过回合可用时间 (1s) ，返回true
    // 此后所有操作将无效
    // 如果 AI 耗时较长，建议经常检查此时限
    
    bool TotalTimeLimitExceeded();
    // 如果已经超过平台等待时间 (1.5s) ，返回true
    // 会被直接判负
    // 绝对不要超过
    
    Soldier GetMap(int x, int y);
    // 返回地图上一个位置的单位信息
    // 对处于你真实视野范围外的幻像，将返回对应类型的满血单位的属性（包括行动点数，不会返回4，而是2 or 3）
    // 对于你真实视野外的潜行单位，将返回空 ( x.ownerID = -1 )
    
    state GetMine(int x, int y);
    // 返回地图上一个点的法阵信息
    // 只会返回被你占领的法阵，其他一律返回“不存在” ( x.mine == -1)
    
    bool InBase(int x, int y, int id);
    // 返回一个点是否是 id 号玩家的基地
    
    int Dis(int fx, int fy, int tx, int ty);
    // 返回两个点之间的曼哈顿距离
    
    int GetId();
    // 返回你的玩家编号
    // 0表示先手，在左侧
    // 1表示后手，在右侧
    
    int GetHP(int id);
    // 返回编号为 id 的玩家剩余的生命值（初始为5）
    // id 不合法则返回 -1
    
    int GetMoney();
    // 返回你当前的剩余金钱
    
    
    // 以下操作当且仅当成功时返回 true:
    
    bool Move(int fx, int fy, int tx, int ty);
    // 将 (fx, fy) 点的你的单位移动到空地 (tx, ty) 点。
    
    bool Attack(int fx, int fy, int tx, int ty);
    // 让 (fx, fy) 点的你的单位攻击 (tx, ty) 的敌方单位。
    
    bool Build(int fx, int fy, int type, bool real);
    // 在 (fx, fy) 建立一个你的单位，type表示类型，克制关系为 1 > 2 > 3 > 1，此外123相同。4为弓箭手。
    // real为假表示生成幻象。每个回合，每个基地只能调用一次 Build。
    
    bool Hidechange(int fx, int fy);
    // 更改位于 (fx, fy) 单位的潜行状态 （潜行变现形、现形变潜行）
    
    bool UseActPoint(int fx, int fy);
	// 语法糖，可以安全地实现浪费一个兵行动点数的功能。SampleAI实现中有使用到
};
