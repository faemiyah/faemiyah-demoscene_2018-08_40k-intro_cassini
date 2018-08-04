clear;
close all;

x = 0:0.01:2;
y = x.^2;

x = cubicsolver(4, 0, 6, -4);

plot(x,y);
hold on;
plot([2 0.55357], [-1, 0.55357^2], 'k-')
axis([0 3 -1 2])