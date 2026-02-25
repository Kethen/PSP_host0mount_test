#include <pspkernel.h>
#include <pspdebug.h>
#include <string.h>
#include <pspiofilemgr.h>
#include <stdio.h>

PSP_MODULE_INFO("host0 test", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_STACK_SIZE_KB(8192);
PSP_HEAP_SIZE_KB(0);
#define LOG_PATH "ms0:/host0_test.log"

#define LOG(...) { \
	int _fd = sceIoOpen(LOG_PATH, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777); \
	if (_fd > 0){ \
		char _log_buf[2048] = {0}; \
		int _len = sprintf(_log_buf, __VA_ARGS__); \
		sceIoWrite(_fd, _log_buf, _len); \
		sceIoClose(_fd); \
	} \
	pspDebugScreenPrintf(__VA_ARGS__); \
}

static void init_logging(){
	int fd = sceIoOpen(LOG_PATH, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd > 0){
		sceIoClose(fd);
	}
	pspDebugScreenInit();
}

static void test(){
	int dirfd = sceIoDopen("host0:/");
	if (dirfd < 0){
		LOG("%s: failed opening host0:/, 0x%x\n", __func__, dirfd);
		return;
	}

	LOG("%s: host0:/ opened\n", __func__);

	while(1){
		SceIoDirent dir = {0};
		int dread_result = sceIoDread(dirfd, &dir);
		if (dread_result < 0){
			LOG("%s: failed reading from dirfd, 0x%x\n", __func__, dread_result);
			sceIoDclose(dirfd);
			return;
		}
		if (dread_result == 0){
			LOG("%s: end of directory reached\n", __func__);
			sceIoDclose(dirfd);
			break;
		}
		LOG("%s: %s 0x%x\n", __func__, dir.d_name, dir.d_stat.st_mode);
	}
}

static int test_thread_func(SceSize args, void *argp){
	LOG("%s: running test in new thread\n", __func__);
	test();
	return 0;
}

static void test_chdir(const char *path){
	LOG("%s: running sceIoChdir, switching to [%s]\n", __func__, path);
	int chdir_result = sceIoChdir(path);
	if (chdir_result < 0){
		LOG("%s: sceIoChdir to [%s] failed, 0x%x\n", __func__, path, chdir_result);
	}
}

static void run_test(){
	LOG("%s: running test in main thread\n", __func__);
	test();

	int tid = sceKernelCreateThread("test_thread", test_thread_func, 0x18, 8192, 0, NULL);
	sceKernelStartThread(tid, 0, NULL);
	sceKernelWaitThreadEnd(tid, NULL);
	sceKernelTerminateDeleteThread(tid);
}

int main(int argc, char *argv[])
{
	init_logging();
	LOG("host0 test\n");

	run_test();

	test_chdir("ms0:/bsb");

	run_test();

	test_chdir("ms0:/psp");

	LOG("%s: running test in main thread\n", __func__);

	run_test();

	LOG("Exiting in 5 seconds...\n");
	sceKernelDelayThread(5000000);
	sceKernelExitGame();
    return 0;
}

