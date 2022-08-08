#if defined(__GNUC__)
#  define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#  if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)
#    define GCC_CXX11
#  endif
#  if (GCC_VERSION < 40600) || !defined(GCC_CXX11)
#    define NO_CXX11_NULLPTR
#  endif
#endif


#if defined(NO_CXX11_NULLPTR)
#message("GCC does not support nullptr")
#message("Redefine nullptr as NULL")
#define nullptr NULL
#endif

