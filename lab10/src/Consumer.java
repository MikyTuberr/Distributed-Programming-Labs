import java.util.concurrent.ThreadLocalRandom;

public class Consumer implements Runnable {
    private final Warehouse warehouse;
    private final int minConsumptionTime;
    private final int maxConsumptionTime;

    public Consumer(Warehouse warehouse, int minConsumptionTime, int maxConsumptionTime) {
        this.warehouse = warehouse;
        this.minConsumptionTime = minConsumptionTime;
        this.maxConsumptionTime = maxConsumptionTime;
    }

    @Override
    public void run() {
        while (warehouse.isRunning()) {
            Products product = Products.values()[ThreadLocalRandom.current().nextInt(Products.values().length)];
           // int quantity = ThreadLocalRandom.current().nextInt(1, 6);
            try {
                int takenQuantity = warehouse.takeProduct(product, 1);
                //System.out.println(Thread.currentThread().getName() + " consumed " + 1 + " of " + product);
                Thread.sleep(ThreadLocalRandom.current().nextInt(minConsumptionTime, maxConsumptionTime));
            } catch (InterruptedException e) {
                System.out.println(Thread.currentThread().getName() + " shutting down...");
                return;
            }
        }
        System.out.println(Thread.currentThread().getName() + " shutting down...");
    }
}
