#ifndef BSBS_HELPER_H_
#define BSBS_HELPER_H_

#define Error(msg, ...) ({printf("Error: "msg"\n", __VA_ARGS__); exit(EXIT_FAILURE);})
#define ErrorLine(lineno, msg, ...)										\
	({printf("Error at line %d: "msg"\n", lineno, __VA_ARGS__); exit(EXIT_FAILURE);})

unsigned int Bsbs_Hash(char *buffer) {
	// sdbm hash
	unsigned long long hash = 0;
	for (unsigned int i = 0; i < strlen(buffer); ++i)
		hash = buffer[i] + (hash<<6) + (hash<<16) - hash;
	return hash;
}

#endif // BSBS_HELPER_H_
