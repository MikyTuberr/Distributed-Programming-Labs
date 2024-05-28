package zad2;

import java.util.concurrent.*;

public class CachingPrimeChecker {
    private final ConcurrentMap<Long, Future<Boolean>> cache = new ConcurrentHashMap<>();
    private final ExecutorService executor = Executors.newSingleThreadExecutor();

    public boolean isPrime(final long x) {
        try {
            Future<Boolean> future = cache.computeIfAbsent(x, this::computeIfIsPrimeAsync);
            return future.get();
        } catch (InterruptedException | ExecutionException e) {
            throw new RuntimeException(e);
        }
    }

    private Future<Boolean> computeIfIsPrimeAsync(final long x) {
        Callable<Boolean> task = () -> computeIfIsPrime(x);
        return executor.submit(task);
    }

    private boolean computeIfIsPrime(long x) {
        final String currentThreadName = Thread.currentThread().getName();
        System.out.printf("\t[%s] Running computation for: %d%n", currentThreadName, x);
        if (x < 2) {
            return false;
        }
        for (long i = 2; i * i <= x; i++) {
            if (x % i == 0) {
                return false;
            }
        }
        return true;
    }

    public void shutdown() {
        executor.shutdown();
    }
}
