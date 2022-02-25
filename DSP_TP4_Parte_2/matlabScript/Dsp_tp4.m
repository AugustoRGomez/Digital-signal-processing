%% TRABAJO PRACTICO N° 4: FILTROS ADAPTATIVOS EN FRDM-K64F
%Integrantes:
%   -GOMEZ, Augusto
%   -ALANIZ, Darío
%   -FERRARIS, Domingo

%PARTE 1: System ID
%leer archivos binarios 

clear; clc;
samplesLen = 256;
numTaps = 30;

fileID = fopen('lmsCoeffSamples.bin');
A = fread(fileID, [samplesLen, numTaps], 'int16');

fileID = fopen('plantaFir.bin');
B = fread(fileID, 'int16');

fileID = fopen('mseArray.bin');
C = fread(fileID, 'int64');

%Graficar Coeficientes
%Coeficientes LMS pares
figure;
for i = 2:2:numTaps
    plot(A(:,i),'LineWidth',2);
    hold on
end
grid on
title("Evolucion coeficientes pares de filtro adaptivo LMS");

% Error
%Tabla error
samples = ones(samplesLen,1);
e = zeros(numTaps,samplesLen);

for i = 1:30
    e(i,:) = (B(i)*samples - A(:,i)); 
end

figure
for i = 2:2:numTaps
    plot(e(i,:),'LineWidth',2);
    hold on
end
grid on;
title("Evolucion error e_i[n] = b_{FIR} - b_{LMS}[n]");

% Mean Square Error
figure;
plot(C,'LineWidth',2);
grid on;
title("Mean square error");


%% PARTE 2: Active Noise Cancellation
% Comparacion Respuesta al impulso FIR con LMS
fileID = fopen('lmsCoeff.bin');
D = fread(fileID, 'int16');

fileID = fopen('plantaFir.bin');
B = fread(fileID, 'int16');

figure
plot(B,'LineWidth',2)
hold on 
plot(D,'LineWidth',2)
grid on
legend('Filtro FIR','Filtro Adaptivo LMS')

%% PARTE 2: Esquema que no funcionó 
% influye el filtro en el proceso de aprendizaje del LMS?
N = 10e3;
x = ones(1,N);
y = ones(1,N);

for i = 1:N
    x(i) = rand;
    y(i) = rand;
end

load h %coeficientes del correlador 
y_conv = conv(x,h);

%Forma de onda
figure
plot(y_conv)
hold on
plot(x)
grid on

%Correlacion
cxy = xcorr(x,y);
cxy_conv = xcorr(x,y_conv);

figure
plot(cxy);
hold on
plot(cxy_conv);
grid on
legend('Corr x y', 'Corr x y_{conv}');
