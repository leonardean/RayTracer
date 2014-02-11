#include <cmath>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include "Config.h"

#pragma warning(push)
#pragma warning(disable:4996)

typedef std::map<SimpleString,SimpleString> VariableSet;
typedef std::set<SimpleString> StringSet;

using namespace std;

static SimpleString preprocess(fstream& InputFile) {
    SimpleString tmp;
    string line;
    while (getline(InputFile,line, '\n')) {
        line = line.substr(0,line.find("//"));
        for (string::iterator i = line.begin(); i != line.end(); ++i) {
            switch (*i) {
            case ' ':case '\t': case '\n': case '\r':
                break;
            default:
              tmp.append(*i);
            }
        }
    }
    return tmp;
}

static bool preload(fstream& InputFile, VariableSet &variables, StringSet &sections) {
    static SimpleString buf;
    SimpleString tmp;
    int recursion=0;
    tmp = preprocess(InputFile);
    SimpleString::iterator i=tmp.begin();
    SimpleString tmpname;
    SimpleString tmpvalue;
    SimpleString currentSection;
    
findname:
    if (i == tmp.end()) {
        goto end;
    }
    switch (*i) {
    case '{':
        if (sections.insert(tmpname).second == false) {
            sections.clear();
            variables.clear();
            return false;
        }
        currentSection = tmpname;
        tmpname.resize(0);
        goto insection;
        break;
    default:
        tmpname.append(&*i,1);
    }
    ++i;
    goto findname;
        
insection:
    if (i == tmp.end()) {
        sections.clear();
        variables.clear();
        return false;
    }
    switch (*i) {
    case '{':
        ++recursion; 
        break;
    case '}':
        --recursion;
        if (recursion == 0) {
            ++i;
            currentSection.resize(0);
            goto findname;
        }
        break;
    default:
        goto variablename;
    }
    ++i;
    goto insection;
    
variablename:
    if (i == tmp.end()) {
            sections.clear();
            variables.clear();
        return false;
    }
    switch (*i) {
    case '{':
        goto insection;
    case '}':
            sections.clear();
            variables.clear();
        return false;
    case '=':
        if (tmpname.empty()) {
            sections.clear();
            variables.clear();
            return false;
        }
        ++i;
        goto variablevalue;
    default:
        tmpname.append(&*i,1);
        break;
    }
    ++i;
    goto variablename;    
    
variablevalue:
    if (i == tmp.end()) {
            sections.clear();
            variables.clear();
        return false;
    }
    switch (*i) {
    case '{':
            sections.clear();
            variables.clear();
        return false;
    case '}':
            sections.clear();
            variables.clear();
        return false;
    case ';':
        if (tmpvalue.empty()) {
            sections.clear();
            variables.clear();
            return false;
        }
        buf.assign(currentSection);
        buf.append('/');
        buf.append(tmpname);
        variables.insert(VariableSet::value_type(buf, tmpvalue));
        tmpname.resize(0);
        tmpvalue.resize(0);
        ++i;
        goto insection;
    default:
        tmpvalue.append(i,1);
        break;
    }
    ++i;
    goto variablevalue;    
end:
    return true;
}

Config::Config(const SimpleString &sFileName) :
m_pVariables(NULL),
m_pSections(NULL),
m_sFileName(sFileName),
m_bLoaded(false)
{
}

Config::~Config()
{
    if (m_pVariables != NULL)
        delete static_cast<VariableSet *>(m_pVariables);
    if (m_pSections != NULL)
        delete static_cast<StringSet *>(m_pSections);
}

int Config::SetSection(const SimpleString &sName)
{
    if (!m_bLoaded) {
        m_bLoaded = true;
        m_pVariables = new VariableSet();
        m_pSections = new StringSet();
        fstream InputFile;
        bool result;
        InputFile.open(m_sFileName.c_str(), ios::in);
        if (InputFile.is_open() == 0) {
            result = false; 
        } else {
            result = preload(InputFile, 
                *(static_cast<VariableSet *>(m_pVariables)),
                *(static_cast<StringSet *>(m_pSections)));
            InputFile.close();
        }
        if (!result) {
            delete static_cast<VariableSet *>(m_pVariables);
            m_pVariables = NULL;
            delete static_cast<StringSet *>(m_pSections);
            m_pSections = NULL;
            return -1;
        }
    }
    if (m_pVariables == NULL) {
        return -1;
    }
    StringSet::const_iterator i = 
        ((StringSet*)m_pSections)->find(sName);
    if (i != ((StringSet*)m_pSections)->end() ) {
        m_sCurrentSection.assign(sName).append('/');
        return 0;
    } else {
        m_sCurrentSection.resize(0);
        return -1;
    }
}

int Config::GetByNameAsInteger(const SimpleString &sName, int lDefaut) const
{
    static SimpleString longName;
    longName.assign(m_sCurrentSection).append(sName);
    VariableSet::const_iterator i = 
        ((VariableSet *)m_pVariables)->find(longName);
    if (i != ((VariableSet *)m_pVariables)->end()) 
        return atoi((*i).second.c_str());
    else {
        return lDefaut;
    }
}

const SimpleString &Config::GetByNameAsString(const SimpleString &sName, const SimpleString &sDefaut) const
{
    static SimpleString longName;
    longName.assign(m_sCurrentSection).append(sName);
    VariableSet::const_iterator i = 
        ((VariableSet *)m_pVariables)->find(longName);
    if (i != ((VariableSet *)m_pVariables)->end()) 
        return (*i).second;
    else {
        return sDefaut;
    }
}

double Config::GetByNameAsFloat(const SimpleString &sName, double fDefaut) const
{
    static SimpleString longName;
    longName.assign(m_sCurrentSection).append(sName);
    VariableSet::const_iterator i = 
        static_cast<VariableSet *>(m_pVariables)->find(longName);
    if (i != ((VariableSet *)m_pVariables)->end()) 
        return atof((*i).second.c_str());
    else {
        return fDefaut;
    }

}

bool Config::GetByNameAsBoolean(const SimpleString &sName, bool bDefaut) const
{
    static SimpleString longName;
    longName.assign(m_sCurrentSection).append(sName);
    VariableSet::const_iterator i = 
        static_cast<VariableSet *>(m_pVariables)->find(longName);
    if (i != ((VariableSet *)m_pVariables)->end()) {
        return (*i).second.compare("true")==0;
    } else {
        return bDefaut;
    }
}

vector3d Config::GetByNameAsVector(const SimpleString &sName, const vector3d& vDefault) const
{
    vector3d tempvector3d;
    static SimpleString longName; 
    longName.assign(m_sCurrentSection).append(sName);
    VariableSet::const_iterator i = 
        static_cast<VariableSet *>(m_pVariables)->find(longName);
    if (i != ((VariableSet *)m_pVariables)->end()) {
        int nbRead = sscanf(i->second.c_str(), "%f,%f,%f", &tempvector3d.x, &tempvector3d.y, &tempvector3d.z);
        if (nbRead != 3) {
            return vDefault;
        }
        
        return tempvector3d;
    } else {
        return vDefault;
    }
}

point Config::GetByNameAsPoint(const SimpleString &sName, const point& ptDefault) const
{
    point tempPoint;
    static SimpleString longName; 
    longName.assign(m_sCurrentSection).append(sName);
    VariableSet::const_iterator i = 
        static_cast<VariableSet *>(m_pVariables)->find(longName);
    if (i != ((VariableSet *)m_pVariables)->end()) {
        int nbRead = sscanf(i->second.c_str(), "%f,%f,%f", &tempPoint.x, &tempPoint.y, &tempPoint.z);
        if (nbRead != 3) {
            return ptDefault;
        }
        
        return tempPoint;
    } else {
        return ptDefault;
    }
}

#pragma warning(pop)
