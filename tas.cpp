#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

#define FILENAME "teamFile.txt"

// 打卡记录类
class Record
{
public:    
    string m_Date;
    string m_comeTime;
    string m_leaveTime;

    Record() 
    : m_Date(""), m_comeTime(""), m_leaveTime("")
    {}

    Record(string date, string comeTime, string leaveTime)
    : m_Date(date), m_comeTime(comeTime), m_leaveTime(leaveTime)
    {}

    void showRecord()
    {
        std::cout << m_Date << " " << m_comeTime << "~" << m_leaveTime << endl;
    }
};

// 成员类
class Member
{
public:
    int m_Id;
    string m_Name;
    int m_Times;

    Record** m_Record;
    int m_workTime;

    Member(int id, string name, int times)
    : m_Id(id), m_Name(name), m_Times(times), m_workTime(0)
    {
        m_Record = new Record*[m_Times];
        for (int i = 0; i < m_Times; i++)
        {
            m_Record[i] = NULL;
        }
    }

    void showInfo()
    {
        std::cout << "学号：" << m_Id
            << "\t姓名：" << m_Name
            << "\t打卡次数：" << m_Times
            << "\t总工作时间：" << timeToStr(m_workTime) << endl;
    }

    void showRecord()
    {
        if (m_Times > 0)
        {
            std::cout << "打卡记录如下：" << endl;
        }
        for (int i = 0; i < m_Times; i++)
        {
            m_Record[i]->showRecord();
        }
    }

    string timeToStr(int workTime)
    {
        int hours = workTime / 60;
        int minutes = workTime % 60;
        ostringstream oss;
        if (hours && minutes)
        {
            oss << hours << "小时" << minutes << "分钟";
        }
        else if (hours)
        {
            oss << hours << "小时";
        }
        else if (minutes)
        {
            oss << minutes << "分钟";
        }
        else{
            oss << "0";
        }
        return oss.str();
    }
};

// 管理类
class AttendanceSystem
{
public:
    // 记录文件中的人数个数
    int m_MemberNum;

    // 成员数组的指针
    Member ** m_MemberArray;

    // 标志文件是否为空
    bool m_FileIsEmpty;

    // 构造函数
    AttendanceSystem();

    // 显示菜单
    void showMenu();

    // 退出系统
    void exitSystem();

    // 添加成员信息
    void addMember();

    // 统计人数
    int getMemberNum();

    // 初始化成员
    void initMember();

    // 将时间字符串转换为分钟数整型
    int  timeToMinute(const string& timeStr);

    // 计算工作时间
    int calcuWorkTime(const string& comeTime, const string& leaveTime);

    // 显示成员信息
    void showMember();

    // 查询成员，显示详细信息
    void findMember();

    // 判断成员是否存在
    int isExist(int id, int range);

    // 保存到本地文件
    void save(string filename = FILENAME);

    // 删除成员
    void delMember();

    // 排序成员及打卡记录
    void sortMember();

    // 转换日期
    void parseDate(const string& date, int& year, int& month, int& day);

    // 比较日期
    bool compareDates(const string& date1, const string& date2);

    // 验证输入日期的合法性
    bool isValidDate(const string& inStr);
};

AttendanceSystem::AttendanceSystem()
{
    ifstream ifs(FILENAME, ios::in);

    if (!ifs.is_open())
    {
        std::cout << "文件不存在" << endl;
        m_FileIsEmpty = true;
        m_MemberNum = 0;
        m_MemberArray = NULL;
        ifs.close();
        return;
    }

    char ch;
    ifs >> ch;
    if (ifs.eof())
    {
        m_FileIsEmpty = true;
        m_MemberNum = 0;
        m_MemberArray = NULL;
        ifs.close();
        return;
    }

    int num = this->getMemberNum();
    m_MemberNum = num;
    m_FileIsEmpty = false;

    m_MemberArray = new Member*[m_MemberNum + 10];
    initMember();
}

void AttendanceSystem::showMenu()
{
    std::cout << endl;
    std::cout << "********************************************" << endl;
    std::cout << "*********  欢迎使用战队考勤系统！ **********" << endl;
    std::cout << "*************  0.退出管理程序  *************" << endl;
    std::cout << "*************  1.显示成员信息  *************" << endl;
    std::cout << "*************  2.增加成员信息  *************" << endl;
    std::cout << "*************  3.删除成员信息  *************" << endl;
    std::cout << "*************  4.查询成员信息  *************" << endl;
    std::cout << "*************  5.导出成员信息  *************" << endl;
    std::cout << "********************************************" << endl;
    std::cout << endl;        
}

void AttendanceSystem::exitSystem()
{
    std::cout << "欢迎下次使用" << endl;
    exit(0);
}

void AttendanceSystem::addMember()
{
    std::cout << "请输入成员学号：" << endl;
    int id;
    std::cin >> id;
    if (id < 1000 || id > 9999)
    {
        cout << "学号格式错误：应满足1000<= id <= 9999" << endl;
        return;
    }
    std::cout << "请输入成员姓名：" << endl;
    string name;
    std::cin >> name;
    int index = isExist(id, m_MemberNum);
    if (index != -1)
    {
        if (m_MemberArray[index]->m_Name != name)
        {
            std::cout << "输入的学号和姓名不匹配" << endl;
            return;
        }
        else
        {
            std::cout << "请输入新增打卡次数：" << endl;
            int times,real_times = 0;
            std::cin >> times;
            if (times < 1)
            {
                cout << "新增打卡次数不能小于1" << endl;
                return;
            }
            int currentTimes = m_MemberArray[index]->m_Times;
            string line, date, timeRange, comeTime, leaveTime;
            std::cin.ignore();
            for (int i = 0; i < times; i++)
            {
                std::cout << "请输入新增第" << i + 1 << "次打卡日期" << endl;
                std::cout << "格式为year.month.day come:time~leave:time" << endl;
                getline(std::cin, line);
                if (!(isValidDate(line)))
                {
                    continue;
                }
                istringstream iss(line);
                iss >> date >> timeRange;
                int pos = timeRange.find('~');
                comeTime = timeRange.substr(0, pos);
                leaveTime = timeRange.substr(pos + 1);
                Record * record = new Record(date, comeTime, leaveTime);
                m_MemberArray[index]->m_Record[currentTimes + i] = record;
                int workTime = calcuWorkTime(comeTime, leaveTime);
                m_MemberArray[index]->m_workTime += workTime;
                real_times++;
            }
            m_MemberArray[index]->m_Times += real_times;
            if (real_times > 0)
            {
                std::cout << "成功新增打卡次数:" << real_times << endl;
            }
        }
    }
    else
    {
        std::cout << "请输入打卡次数：" << endl;
        int times,real_times = 0;
        std::cin >> times;
        if (times < 1)
        {
            cout << "打卡次数不能小于1" << endl;
            return;
        }
        Member ** newSpace = new Member*[m_MemberNum + 1];
        if (m_MemberArray != NULL)
        {
            for (int i = 0; i < m_MemberNum; i++)
            {
                newSpace[i] = m_MemberArray[i];
            }
        }
        Member * mem = new Member(id, name, times);
        newSpace[m_MemberNum] = mem;
        string line, date, timeRange, comeTime, leaveTime;
        std::cin.ignore();
        for (int i = 0; i < times; i++)
        {
            std::cout << "请输入第" << i + 1 << "次打卡日期" << endl;
            std::cout << "格式为year.month.day come:time~leave:time" << endl;
            getline(std::cin, line);
            if (!(isValidDate(line)))
            {
                continue;
            }
            istringstream iss(line);
            iss >> date >> timeRange;
            int pos = timeRange.find('~');
            comeTime = timeRange.substr(0, pos);
            leaveTime = timeRange.substr(pos + 1);
            Record * record = new Record(date, comeTime, leaveTime);
            mem->m_Record[real_times] = record;
            real_times++;
            int workTime = calcuWorkTime(comeTime, leaveTime);
            mem->m_workTime += workTime;
        }
        mem->m_Times = real_times;
        delete[] m_MemberArray;
        m_MemberArray = newSpace;
        m_MemberNum += 1;
        m_FileIsEmpty = false;
        std::cout << "成功添加新成员！" << endl;
    }
    sortMember();
    save();
}

int AttendanceSystem::getMemberNum()
{
    ifstream ifs(FILENAME, ios::in);
    int count = 0;
    string line;
    while (getline(ifs, line))
    {
        count++;
    }

    ifs.close();
    return count;
}

void AttendanceSystem::initMember()
{
    ifstream ifs(FILENAME, ios::in);

    int id;
    string name;
    int times;
    int index = 0;
    string line;
    int cnt = 0,c_index;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        iss >> id >> name >> times;
        if (cnt)
        {
            c_index = isExist(id, index);
        }
        else
        {
            c_index = -1;
            cnt++;
        }
        if (c_index != -1)
        {
            if (m_MemberArray[c_index]->m_Name != name)
            {
                std::cout << "该学号：" << id << " 与原有姓名：" 
                << m_MemberArray[c_index]->m_Name << " 不符：" << name << endl;
                m_MemberNum--;
                continue; 
            }
            else
            {
                int currentTimes = m_MemberArray[c_index]->m_Times,real_times=0;
                string date, timeRange, comeTime, leaveTime;
                while (!iss.eof())
                {
                    iss >> date >> timeRange;
                    string str = date + " " + timeRange;
                    if(!isValidDate(str))
                    {
                        cout << "对于学号为 " << id << " 的成员，有以上信息输入错误" << endl;
                        continue;
                    }
                    int pos = timeRange.find('~');
                    comeTime = timeRange.substr(0, pos);
                    leaveTime = timeRange.substr(pos + 1);
                    Record * record = new Record(date, comeTime, leaveTime);
                    m_MemberArray[c_index]->m_Record[currentTimes + real_times] = record;
                    int workTime = calcuWorkTime(comeTime, leaveTime);
                    m_MemberArray[c_index]->m_workTime += workTime;
                    real_times++;
                }
                m_MemberNum--;
                m_MemberArray[c_index]->m_Times += real_times;
            }
        }
        else
        {
            Member * mem = new Member(id, name, times);
            m_MemberArray[index++] = mem;
            string date, timeRange, comeTime, leaveTime;
            int real_times = 0;
            while (!iss.eof())
            {
                iss >> date >> timeRange;
                string str = date + " " + timeRange;
                if(!isValidDate(str))
                {
                    cout << "对于学号为 " << id << " 的成员，有以上信息输入错误" << endl;
                    continue;
                }
                int pos = timeRange.find('~');
                comeTime = timeRange.substr(0, pos);
                leaveTime = timeRange.substr(pos + 1);
                Record * record = new Record(date, comeTime, leaveTime);
                mem->m_Record[real_times] = record;
                int workTime = calcuWorkTime(comeTime, leaveTime);
                mem->m_workTime += workTime;
                real_times++;
            }
            mem->m_Times = real_times;
        }
    }
    sortMember();
}

int AttendanceSystem::timeToMinute(const string& timeStr)
{
    int hours = stoi(timeStr.substr(0, timeStr.find(':')));
    int minutes = stoi(timeStr.substr(timeStr.find(':') + 1));
    return hours * 60 + minutes;
}

int AttendanceSystem::calcuWorkTime(const string& comeTime, const string& leaveTime)
{
    int comeTimes = timeToMinute(comeTime);
    int leaveTimes = timeToMinute(leaveTime);
    return leaveTimes - comeTimes;
}

void AttendanceSystem::showMember()
{
    if (m_FileIsEmpty)
	{
		std::cout << "文件不存在或记录为空！" << endl;
	}
    else
    {
        if (m_MemberNum < 1)
        {
            cout << "暂无成员信息" << endl;
            return;
        }
        sortMember();
        cout << "所有成员信息如下：" << endl;
        for (int i = 0; i < m_MemberNum; i++)
        {
            this->m_MemberArray[i]->showInfo();
            // this->m_MemberArray[i]->showRecord();
        }
    }
}

void AttendanceSystem::findMember()
{
    if (m_FileIsEmpty)
	{
		std::cout << "文件不存在或记录为空！" << endl;
	}
    else
    {
        std::cout << "请输入要查询的成员学号：" << endl;
        int id = 0;
        std::cin >> id;
        int index = isExist(id, m_MemberNum);
        if (index != -1)
        {
            std::cout << "查询成功！该成员信息如下：" << endl;
            m_MemberArray[index]->showInfo();
            m_MemberArray[index]->showRecord();
        }
        else
        {
            std::cout << "未找到该成员" << endl;
        }
    }
}

int AttendanceSystem::isExist(int id, int range)
{
	int index = -1;
	for (int i = 0; i < range; i++)
	{
		if (this->m_MemberArray[i]->m_Id == id)
		{
			index = i;
			break;
		}
	}
	return index;
}

void AttendanceSystem::save(string filename)
{
    ofstream ofs(filename, ios::out);
    for (int i = 0; i < m_MemberNum; i++)
    {
        ofs << m_MemberArray[i]->m_Id << " "
            << m_MemberArray[i]->m_Name << " "
            << m_MemberArray[i]->m_Times;
        for (int j = 0 ; j < m_MemberArray[i]->m_Times; j++)
        {
            ofs << " " << m_MemberArray[i]->m_Record[j]->m_Date 
                << " " << m_MemberArray[i]->m_Record[j]->m_comeTime << "~"
                << m_MemberArray[i]->m_Record[j]->m_leaveTime;
        }
        ofs << endl;
    }
    ofs.close();
}

void AttendanceSystem::delMember()
{
    if (m_FileIsEmpty)
	{
		std::cout << "文件不存在或记录为空！" << endl;
	}
    else
    {
        std::cout << "请输入要删除成员的学号：" << endl;
        int id = 0;
        std::cin >> id;
        int index = isExist(id, m_MemberNum);
        if (index != -1)
        {
            for (int i = index; i < m_MemberNum; i++)
            {
                m_MemberArray[i] = m_MemberArray[i + 1];
            }
            m_MemberNum--;
            save();
            std::cout << "删除成功！" << endl;
        }
        else
        {
            std::cout << "删除失败，未找到该成员" << endl;
        }
    }
}

void AttendanceSystem::sortMember()
{
    if (m_FileIsEmpty)
	{
		std::cout << "文件不存在或记录为空！" << endl;
	}
    else
    {
        for (int i = 0; i < m_MemberNum; i++)
        {
            for (int j = i + 1; j < m_MemberNum; j++)
            {
                if (m_MemberArray[i]->m_Id > m_MemberArray[j]->m_Id)
                {
                    Member * temp = m_MemberArray[i];
                    m_MemberArray[i] = m_MemberArray[j];
                    m_MemberArray[j] = temp;
                }
            }
        }
        for (int i = 0; i < m_MemberNum; i++)
        {
            for (int j = 0; j < m_MemberArray[i]->m_Times; j++)
            {
                for (int k = j + 1; k < m_MemberArray[i]->m_Times; k++)
                {
                    if (compareDates(m_MemberArray[i]->m_Record[j]->m_Date, m_MemberArray[i]->m_Record[k]->m_Date))
                    {
                        Record * temp = m_MemberArray[i]->m_Record[j];
                        m_MemberArray[i]->m_Record[j] = m_MemberArray[i]->m_Record[k];
                        m_MemberArray[i]->m_Record[k] = temp;
                    }
                }
            }
        }
        save();
    }
}

void AttendanceSystem::parseDate(const string& date, int& year, int& month, int& day)
{
    istringstream iss(date);
    char dot;
    iss >> year >> dot >> month >> dot >> day;
}

bool AttendanceSystem::compareDates(const string& date1, const string& date2)
{
    int year1, month1, day1;
    int year2, month2, day2;
    parseDate(date1, year1, month1, day1);
    parseDate(date2, year2, month2, day2);
    if (year1 > year2)
    {
        return true;
    }else if (year1 < year2)
    {
        return false;
    }else
    {
        if (month1 > month2)
        {
            return true;
        }else if (month1 < month2)
        {
            return false;
        }
        else
        {
            if (day1 > day2)
            {
                return true;
            }else if (day1 < day2)
            {
                return false;
            }
            else
            {
                return false;
            }
        }
    }
}

bool AttendanceSystem::isValidDate(const string& inStr)
{
    istringstream iss(inStr);
    int year,month,day,inHour,inMin,leHour,leMin;
    char dot1,dot2,colon1,dash,colon2;
    iss >> year >> dot1 >> month >> dot2 >> day 
        >> inHour >> colon1 >> inMin >> dash >> leHour >> colon2 >> leMin;
    if (dot1 != '.' || dot2 != '.' || colon1 != ':' || dash != '~' || colon2 != ':')
    {
        cout << "请按照格式：year.month.day come:time~leave:time 输入：" << inStr << endl;
        return false;
    }
    if (year < 0 || month < 1 || month > 12 || day < 1 || day > 30)
    {
        cout << "请输入正确的日期：" << inStr << endl;
        return false;
    }
    if (inHour < 0 || inHour > 23 || inMin < 0 || inMin > 59)
    {
        cout << "请输入正确的打卡时间：" << inStr << endl;
        return false;
    }
    if (leHour < 0 || leHour > 23 || leMin < 0 || leMin > 59)
    {
        cout << "请输入正确的打卡时间：" << inStr << endl;
        return false;
    }
    if (inHour > leHour)
    {
        cout << "请输入正确的打卡时间：" << inStr << endl;
        return false;
    }
    else if (inHour == leHour)
    {
        if (inMin >= leMin)
        {
            cout << "请输入正确的打卡时间：" << inStr << endl;
            return false;
        }
    }
    return true;
}


int main()
{
    AttendanceSystem as;
    int choice;
    string filename;
    while(true)
    {
        as.showMenu();
        std::cout << "请输入要进行的操作：" << endl;
        std::cin >> choice;
        switch (choice)
        {
        case 0: // 退出系统
            as.exitSystem();
            break;
        case 1: // 显示成员信息
            as.showMember();
            break;
        case 2: // 增加成员信息
            as.addMember();
            break;
        case 3: // 删除成员信息
            as.delMember();
            break;
        case 4: // 查询成员信息
            as.findMember();
            break;
        case 5: // 导出成员信息
            std::cout << "请输入要导入的文件名称：" <<endl;
            std::cin >> filename;
            as.save(filename);
            std::cout << "导出成功！" << endl;
            break;
        default:
            std::cout << "无效的选项，请重新输入。" << endl;
            break;
        }
    }
    return 0;
}
