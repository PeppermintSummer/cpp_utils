from dataclasses import dataclass
import time

@dataclass
class AutoTimer:
    def __init__(self, name) -> None:
        self.m_start_time = None # get now
        self.m_task_name  = name
    
    def start(self):
        if self.m_start_time is not None:
            raise TimerError(f'Timer is running. Use .stop() to stop it')
        self.m_start_time = time.perf_counter()
    
    def stop(self):
        if self.m_start_time is None:
            raise TimerError(f"Timer is not running. Use .start() to start it")
        elapsed_time = time.perf_counter() - self.m_start_time
        self.m_start_time = None
        print(f"**********{self.m_task_name:s} take time: {elapsed_time:0.4f} seconds**********")
    
    def __enter__(self):
        self.start()
        return self
    
    def __exit__(self, *exc_info):
        self.stop()
        return self

if __name__ == '__main__':
    with AutoTimer('download'):
        time.sleep(5)
        print('task done')
    print('all done')