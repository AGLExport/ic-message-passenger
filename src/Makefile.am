AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

bin_PROGRAMS = ms_server ms_client

ms_server_SOURCES = \
		ms_server.c 

# options
# Additional library
ms_server_LDADD = \
		-lrt -lpthread 

# C compiler options
ms_server_CFLAGS = \
		-I$(top_srcdir)/include \
		-D_GNU_SOURCE 

# Linker options
ms_server_LDFLAGS = 



ms_client_SOURCES = \
		ms_client.c 

# options
# Additional library
ms_client_LDADD = \
		-lrt -lpthread 

# C compiler options
ms_client_CFLAGS = \
		-I$(top_srcdir)/include 

# Linker options
ms_client_LDFLAGS = 


# configure option 
#if ENABLE_USERDEBUG
#exsample_CFLAGS   += -D_USER_DEBUG_
#exsample_CXXFLAGS += -D_USER_DEBUG_
#endif
#
#if ENABLE_PRINTFDEBUG
#exsample_CFLAGS   += -D_PRINTF_DEBUG_
#exsample_CXXFLAGS += -D_PRINTF_DEBUG_
#endif

