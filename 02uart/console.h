#ifndef CONS_H
#define CONS_H
class console {
private:
	void printnum(unsigned long long num, unsigned base);
public:
	console() {};
	virtual void putc(unsigned char ch) = 0;
	virtual unsigned char getc() = 0;
	int cprintf(const char *fmt, ...);
};
#endif
