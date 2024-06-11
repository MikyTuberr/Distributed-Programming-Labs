import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Warehouse {
    private final Map<Products, Integer> products = new HashMap<>();
    private final int capacity;
    private volatile boolean running = true;

    public Warehouse(int capacity) {
        this.capacity = capacity;
    }

    public synchronized boolean isFull() {
        return products.size() >= capacity;
    }

    public synchronized boolean isEmpty(Products product) {
        if (products.containsKey(product)) {
            return products.get(product) <= 0;
        }
        return false;
    }

    public synchronized void addProduct(Products product, int quantity) throws InterruptedException {
        while (isFull()) {
            wait();
        }
        for (int i = 0; i < quantity; i++) {
            if (!isFull()) {
                if (products.containsKey(product)) {
                    products.put(product, products.get(product) + 1);
                } else {
                    products.put(product, 1);
                }
            }
        }
        notifyAll();
    }

    public synchronized int takeProduct(Products product, int quantity) throws InterruptedException {
        int takenQuantity = 0;
        while (isEmpty(product) && running) {
            wait();
        }
        for (int i = 0; i < quantity; i++) {
            if (!isEmpty(product)) {
                products.remove(product);
                takenQuantity++;
            }
        }
        notifyAll();
        return takenQuantity;
    }

    public synchronized boolean isRunning() {
        return running;
    }

    public void stop() {
        this.running = false;
    }

    public synchronized int getProductCount(Products product) {
        if (products.containsKey(product)) {
            return products.get(product);
        }
        return 0;
    }

    public synchronized int getProductsCount() {
        return products.size();
    }
}
