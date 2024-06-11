import java.util.concurrent.ThreadLocalRandom;

public class Producer implements Runnable {
    private final Warehouse warehouse;
    private final int minProductionTime;
    private final int maxProductionTime;

    public Producer(Warehouse warehouse, int minProductionTime, int maxProductionTime) {
        this.warehouse = warehouse;
        this.minProductionTime = minProductionTime;
        this.maxProductionTime = maxProductionTime;
    }

    @Override
    public void run() {
        while (warehouse.isRunning()) {
            Products product = Products.values()[ThreadLocalRandom.current().nextInt(Products.values().length)];
            //int quantity = ThreadLocalRandom.current().nextInt(1, 11);
            try {
                warehouse.addProduct(product, 1);
                System.out.println(Thread.currentThread().getName() + " produced " + 1 + " of " + product);
                Thread.sleep(ThreadLocalRandom.current().nextInt(minProductionTime, maxProductionTime));
            } catch (InterruptedException e) {
                System.out.println(Thread.currentThread().getName() + " shutting down...");
                return;
            }
        }
        System.out.println(Thread.currentThread().getName() + " shutting down...");
    }
}
