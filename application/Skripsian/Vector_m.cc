//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/application/Skripsian/Vector.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Vector_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

Register_Class(myDataPacket);

myDataPacket::myDataPacket(const char *name, int kind) : ::ApplicationPacket(name,kind)
{
    myDataArray_arraysize = 0;
    this->myDataArray_var = 0;
}

myDataPacket::myDataPacket(const myDataPacket& other) : ::ApplicationPacket(other)
{
    myDataArray_arraysize = 0;
    this->myDataArray_var = 0;
    copy(other);
}

myDataPacket::~myDataPacket()
{
    delete [] myDataArray_var;
}

myDataPacket& myDataPacket::operator=(const myDataPacket& other)
{
    if (this==&other) return *this;
    ::ApplicationPacket::operator=(other);
    copy(other);
    return *this;
}

void myDataPacket::copy(const myDataPacket& other)
{
    delete [] this->myDataArray_var;
    this->myDataArray_var = (other.myDataArray_arraysize==0) ? NULL : new double[other.myDataArray_arraysize];
    myDataArray_arraysize = other.myDataArray_arraysize;
    for (unsigned int i=0; i<myDataArray_arraysize; i++)
        this->myDataArray_var[i] = other.myDataArray_var[i];
}

void myDataPacket::parsimPack(cCommBuffer *b)
{
    ::ApplicationPacket::parsimPack(b);
    b->pack(myDataArray_arraysize);
    doPacking(b,this->myDataArray_var,myDataArray_arraysize);
}

void myDataPacket::parsimUnpack(cCommBuffer *b)
{
    ::ApplicationPacket::parsimUnpack(b);
    delete [] this->myDataArray_var;
    b->unpack(myDataArray_arraysize);
    if (myDataArray_arraysize==0) {
        this->myDataArray_var = 0;
    } else {
        this->myDataArray_var = new double[myDataArray_arraysize];
        doUnpacking(b,this->myDataArray_var,myDataArray_arraysize);
    }
}

void myDataPacket::setMyDataArrayArraySize(unsigned int size)
{
    double *myDataArray_var2 = (size==0) ? NULL : new double[size];
    unsigned int sz = myDataArray_arraysize < size ? myDataArray_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        myDataArray_var2[i] = this->myDataArray_var[i];
    for (unsigned int i=sz; i<size; i++)
        myDataArray_var2[i] = 0;
    myDataArray_arraysize = size;
    delete [] this->myDataArray_var;
    this->myDataArray_var = myDataArray_var2;
}

unsigned int myDataPacket::getMyDataArrayArraySize() const
{
    return myDataArray_arraysize;
}

double myDataPacket::getMyDataArray(unsigned int k) const
{
    if (k>=myDataArray_arraysize) throw cRuntimeError("Array of size %d indexed by %d", myDataArray_arraysize, k);
    return myDataArray_var[k];
}

void myDataPacket::setMyDataArray(unsigned int k, double myDataArray)
{
    if (k>=myDataArray_arraysize) throw cRuntimeError("Array of size %d indexed by %d", myDataArray_arraysize, k);
    this->myDataArray_var[k] = myDataArray;
}

class myDataPacketDescriptor : public cClassDescriptor
{
  public:
    myDataPacketDescriptor();
    virtual ~myDataPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(myDataPacketDescriptor);

myDataPacketDescriptor::myDataPacketDescriptor() : cClassDescriptor("myDataPacket", "ApplicationPacket")
{
}

myDataPacketDescriptor::~myDataPacketDescriptor()
{
}

bool myDataPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<myDataPacket *>(obj)!=NULL;
}

const char *myDataPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int myDataPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int myDataPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *myDataPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "myDataArray",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int myDataPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "myDataArray")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *myDataPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "double",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *myDataPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int myDataPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    myDataPacket *pp = (myDataPacket *)object; (void)pp;
    switch (field) {
        case 0: return pp->getMyDataArrayArraySize();
        default: return 0;
    }
}

std::string myDataPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    myDataPacket *pp = (myDataPacket *)object; (void)pp;
    switch (field) {
        case 0: return double2string(pp->getMyDataArray(i));
        default: return "";
    }
}

bool myDataPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    myDataPacket *pp = (myDataPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setMyDataArray(i,string2double(value)); return true;
        default: return false;
    }
}

const char *myDataPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *myDataPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    myDataPacket *pp = (myDataPacket *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


