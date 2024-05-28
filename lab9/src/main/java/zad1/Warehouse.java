package zad1;

import zad1.Products;

import java.util.ArrayList;

public class Warehouse {
    private final ArrayList<Products> products = new ArrayList<>();
    private final int capacity = 1000;

    private volatile boolean running = true;

    public synchronized boolean isFull() {
        return products.size() >= capacity;
    }

    public synchronized void addProduct(Products product, int quantity) throws InterruptedException {
        while (isFull()) {
            wait();
        }
        for (int i = 0; i < quantity; i++) {
            if(!isFull())
                products.add(product);
        }
        notifyAll();
    }

    public synchronized int takeProduct(Products product, int quantity) throws InterruptedException {
        int takenQuantity = 0;
        for (int i = 0; i < products.size(); i++) {
            if (products.get(i) == product) {
                products.remove(i);
                takenQuantity++;
                if (takenQuantity == quantity) {
                    break;
                }
            }
        }
        notifyAll();
        return takenQuantity;
    }

    public synchronized boolean getRunning() {
        return running;
    }

    public void stop() {
        this.running = false;
    }
}
