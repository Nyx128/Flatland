#ifdef FL_EXPORT
#define FL_API __declspec(dllexport)
#else
#define FL_API __declspec(dllimport)
#endif
