#ifndef BSBS_HELPER_H_
#define BSBS_HELPER_H_

#define Error(msg, ...) ({printf("%s:%d:0: function: %s\nerror: "msg"\n", \
				__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); exit(EXIT_FAILURE);})
#define ErrorLine(lineno, msg, ...) ({printf("%s:%d:0: function: %s\n%zu:0: error: "msg"\n", \
				__FILE__, __LINE__, __FUNCTION__, lineno, __VA_ARGS__); exit(EXIT_FAILURE);})

unsigned int Bsbs_Hash(char *buffer) {
	// sdbm hash
	unsigned long long hash = 0;
	for (unsigned int i = 0; i < strlen(buffer); ++i)
		hash = buffer[i] + (hash<<6) + (hash<<16) - hash;
	return hash;
}

#endif // BSBS_HELPER_H_
