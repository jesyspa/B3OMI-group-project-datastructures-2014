// This code is from:
// http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4031
// Modified to work in template container classes.

#if (defined(__BORLANDC__) && (!defined(NOPROP)))
// Borland C++ properties
#define PROPERTY_TYPE_GET(vtype,getfunc,name) \
  __property vtype name = { read = getfunc };
#define PROPERTY_TYPE_SET(vtype,setfunc,name) \
  __property vtype name = { write = setfunc };
#define PROPERTY_TYPE(vtype,getfunc,setfunc,name) \
  __property vtype name = { read = getfunc, write = setfunc };

#define InitProp(propname)

#elif (defined(_MSC_VER) && (!defined(NOPROP)))
// MSVC++ properties

#define PROPERTY_TYPE_GET(vtype,getfunc,name) \
        __declspec( property( get=getfunc ) ) vtype name;
#define PROPERTY_TYPE_SET(vtype,setfunc,name) \
        __declspec( property( put=setfunc ) ) vtype name;
#define PROPERTY_TYPE(vtype,getfunc,setfunc,name) \
        __declspec( property( get=getfunc, put=setfunc ) ) vtype name;

#define InitProp(propname)

#else
// This is the generic version of properties.
// Works on all compilers.
// Property macros taken from:
// http://www.codeproject.com/kb/cpp/genericproperty.aspx
// It was updated to fit current requirements of the skiplist library.
#define InitProp(propname) \
  , propname(this)

#define PROPERTY_TYPE_GET(vtype,get,name) \
class property_##name \
{ \
public: \
property_##name() \
{ \
  m_cObject = NULL; \
} \
\
property_##name(self_type* cObject) \
{ \
  m_cObject = cObject; \
} \
/*-- This to set a pointer to the class that contain the */ \
/*   property -- */ \
void setContainer(self_type* cObject) \
{ \
  m_cObject = cObject; \
} \
/*-- To make possible to cast the property class to the */ \
/*   internal type -- */ \
operator vtype() const \
{ \
  assert(m_cObject != NULL); \
  return m_cObject->get(); \
} \
private: \
  self_type* m_cObject;  /*-- Pointer to the module that */ \
                         /*   contains the property -- */ \
} name;

#define PROPERTY_TYPE_SET(vtype,set,name) \
class property_##name \
{ \
public: \
property_##name() \
{ \
  m_cObject = NULL; \
} \
\
property_##name(self_type* cObject) \
{ \
  m_cObject = cObject; \
} \
/*-- This to set a pointer to the class that contain the */ \
/*   property -- */ \
void setContainer(self_type* cObject) \
{ \
  m_cObject = cObject; \
} \
/*-- Overload the '=' sign to set the value using the set */ \
/*   member -- */ \
vtype operator =(const vtype& value) \
{ \
  assert(m_cObject != NULL); \
  m_cObject->set(value); \
  return value; \
} \
private: \
  self_type* m_cObject;  /*-- Pointer to the module that */ \
                         /*   contains the property -- */ \
} name;



#define PROPERTY_TYPE(vtype,get,set,name) \
class property_##name \
{ \
public: \
property_##name() \
{ \
  m_cObject = NULL; \
} \
\
property_##name(self_type* cObject) \
{ \
  m_cObject = cObject; \
} \
/*-- This to set a pointer to the class that contain the */ \
/*   property -- */ \
void setContainer(self_type* cObject) \
{ \
  m_cObject = cObject; \
} \
/*-- To make possible to cast the property class to the */ \
/*   internal type -- */ \
operator vtype() const \
{ \
  assert(m_cObject != NULL); \
  return m_cObject->get(); \
} \
/*-- Overload the '=' sign to set the value using the set */ \
/*   member -- */ \
vtype operator =(const vtype& value) \
{ \
  assert(m_cObject != NULL); \
  m_cObject->set(value); \
  return value; \
} \
private: \
  self_type* m_cObject;  /*-- Pointer to the module that */ \
                         /*   contains the property -- */ \
} name;

#endif
