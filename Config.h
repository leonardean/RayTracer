#ifndef __CONFIG_H
#define __CONFIG_H


#include "Definition.h"
#include "SimpleString.h"
#pragma warning( push )
#pragma warning( disable : 4512 )

class Config {
private:
    void * m_pVariables;
    void * m_pSections;
    const SimpleString m_sFileName;
    SimpleString m_sCurrentSection;
    bool m_bLoaded;
public:
    bool GetByNameAsBoolean(const SimpleString  & sName, bool bDefault) const;
    double GetByNameAsFloat(const SimpleString & sName, double fDefault) const;
    const SimpleString &GetByNameAsString(const SimpleString  &sName, const SimpleString  & sDefault) const;
    int GetByNameAsInteger(const SimpleString  &sName, int lDefault) const;
    vector3d GetByNameAsVector(const SimpleString &sName, const vector3d& vDefault) const;
    point GetByNameAsPoint(const SimpleString &sName, const point& ptDefault) const;
    
    int SetSection(const SimpleString &sName);
    ~Config();
    Config(const SimpleString &sFileName);
};

#pragma warning( pop ) 
#endif 
