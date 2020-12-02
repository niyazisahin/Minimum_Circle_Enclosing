#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

/*
DERLEME KODU
g++ main.c -lbgi -lgdi32 -lcomdlg32 -luuid -loleaut32 -lole32 -static-libgcc -static-libstdc++
*/

// nokta verileri bu structa tutulur
typedef struct Nokta
{
    float x;
    float y;
} nokta;

typedef struct Daire
{
    nokta merkez;
    float r;
} daire;

// iki nokta arası uzunluğu hesaplar
float uzunluk(nokta a, nokta b) // --> O(1) fonksiyon
{
    return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2)); // O(1)
}

int compfunc(const void *a, const void *b) // --> O(1) fonksiyon
{
    return ((nokta *)a)->x - ((nokta *)b)->x; // O(1)
}

int factor(int n)
{
    if (n == 1 || n == 0)
    {
        return 1;
    }
    return n * factor(n - 1);
}

int kombinasyon(int n, int r)
{
    return factor(n) / (factor(r) * factor(n - r));
}

float B(int i, int n, float t)
{
    return kombinasyon(n, i) * pow(t, i) * pow((1 - t), n - i);
}

void bspline(nokta *liste, nokta merkez, int n)
{
    float xt = 0.0, yt = 0.0;

    for (float t = 0.0; t <= 1; t += 0.0001)
    {
        for (int i = 0; i < n; i++)
        {

            xt += liste[i].x * B(i, n - 1, t);
            yt += liste[i].y * B(i, n - 1, t);
        }
        //printf("%f %f\n", xt, yt);
        putpixel(merkez.x + (int)xt, merkez.y - (int)yt, 4);
        xt = 0.0;
        yt = 0.0;
    }
}

daire yeni_daire(nokta a, nokta b, nokta c)
{
    // http://www.ambrsoft.com/trigocalc/circle3d.htm

    float A = (a.x * (b.y - c.y)) - (a.y * (b.x - c.x)) + (b.x * c.y) - (c.x * b.y);
    float B = (pow(a.x, 2) + pow(a.y, 2)) * (c.y - b.y) + (pow(b.x, 2) + pow(b.y, 2)) * (a.y - c.y) + (pow(c.x, 2) + pow(c.y, 2)) * (b.y - a.y);
    float C = (pow(a.x, 2) + pow(a.y, 2)) * (b.x - c.x) + (pow(b.x, 2) + pow(b.y, 2)) * (c.x - a.x) + (pow(c.x, 2) + pow(c.y, 2)) * (a.x - b.x);
    //float D = (pow(a.x,2)+pow(a.y,2))*(c.x*b.y)-(b.x*c.y)+(pow(b.x,2)+pow(b.y,2))*((a.x*c.y)-(c.x*a.y))+(pow(c.x,2)+pow(c.y,2))*((b.x*a.y)-(a.x*b.y));
    float x = -B / (2 * A);
    float y = -C / (2 * A);
    float r = sqrt(pow(x - a.x, 2) + pow(y - a.y, 2));
    daire yeni_daire = {{x, y}, r};
    return yeni_daire;
}

int isall_inside(int n, nokta *liste, daire c)
{

    for (int i = 0; i < n; i++)
    {
        if (uzunluk(c.merkez, liste[i]) > c.r)
            return 0;
    }
    return 1;
}

// daire ciz
void draw_circle(daire c, nokta merkez)
{
    setcolor(0);
    circle((merkez.x + c.merkez.x), (merkez.y - c.merkez.y), c.r);
}

void draw_point(nokta p, nokta merkez, int color)
{
    float x = merkez.x + p.x, y = merkez.y - p.y;
    setcolor(0);

    // draw text below point
    char kord[20];
    sprintf(kord, "(%0.f,%0.f)", p.x, p.y);
    settextstyle(SMALL_FONT, 0, 5);
    outtextxy(x + 10, y + 10, kord);

    // draw the point
    setcolor(color);
    setfillstyle(rand() % 12, color);
    fillellipse(x, y, 7, 7);
}

void hesapla(int n, nokta *liste, nokta merkez)
{
    daire big_circle = {{0, 0}, 0};

    if (n < 2)
        return;

    // dairelerein ikili ikili kontrolü
    daire tmp = {{0, 0}, 0};
    float max = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j == i)
                continue;
            float u = uzunluk(liste[i], liste[j]);
            if (u > max)
            { // cap kontrolu
                max = u;
                tmp.merkez.x = (liste[i].x + liste[j].x) / 2;
                tmp.merkez.y = (liste[i].y + liste[j].y) / 2;
                tmp.r = max / 2;
                if (isall_inside(n, liste, tmp))
                    big_circle = tmp;
            }
        }
    }

    // dairelerin üclü üclü kontrolü
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                // eger aynı noktaları kapsıyorsa atla
                if (i == j || i == k || j == k)
                    continue;
                tmp = yeni_daire(liste[i], liste[j], liste[k]);

                if (isall_inside(n, liste, tmp) && (big_circle.r > tmp.r || big_circle.r == 0))
                    big_circle = tmp;
            }
        }
    }

    char m[50];
    sprintf(m, "Daire merkez: (%0.f,%0.f)-Daire yaricap: %0.f", big_circle.merkez.x, big_circle.merkez.y, big_circle.r);
    outtextxy(0, 0, m);

    printf("Daire merkez: %f, %f\n", big_circle.merkez.x, big_circle.merkez.y);
    printf("Daire yaricap: %f\n", big_circle.r);

    draw_point(big_circle.merkez, merkez, 10);
    draw_circle(big_circle, merkez);
}

void kordinat_ciz(int en, int boy)
{
    // change background color
    setbkcolor(15);
    cleardevice();
    setcolor(0);
    setlinestyle(0, 0, 2);

    // draw the coordinate system

    // bu kısmın amacı çizgilerin pencere kenarından biraz daha içerde olmasını sağlamak
    // bunlar en ve boy kullanılarak dinamik bir şekilde hesaplanıyor

    int yx = en / 20;   // yatay nokta baslangıc örn: 100/20 = 5 5pixel içeri
    int ysx = en - yx;  // yatay nokta bitiş
    int dy = boy / 20;  // dikey nokta başlangıç
    int dsy = boy - dy; // dikey nokta bitiş

    line(en / 2, dy, en / 2, dsy);
    line(yx, boy / 2, ysx, boy / 2);

    //ust ok
    line(en / 2, dy, en / 2 + 10, dy + 10);
    line(en / 2, dy, en / 2 - 10, dy + 10);

    // alt ok
    line(en / 2, dsy, en / 2 + 10, dsy - 10);
    line(en / 2, dsy, en / 2 - 10, dsy - 10);

    // sol ok
    line(yx, boy / 2, yx + 10, boy / 2 - 10);
    line(yx, boy / 2, yx + 10, boy / 2 + 10);

    // sag ok
    line(ysx, boy / 2, ysx - 10, boy / 2 - 10);
    line(ysx, boy / 2, ysx - 10, boy / 2 + 10);

    // ysx-yx = en uzunluk
    setcolor(0);
    settextstyle(SMALL_FONT, 0, 5);
    //Yatay Çizgiler.
    for (int i = yx + 5; i < ysx; i += 5)
    {
        if ((i - yx) % 20 == 0)
        {
            setlinestyle(0, 0, 2);
            line(i, (boy / 2) - 5, i, (boy / 2) + 5);
        }
        else
        {
            setlinestyle(0, 0, 1);
            line(i, (boy / 2) - 2, i, (boy / 2) + 2);
        }
    }

    //Dikey Çizgiler
    for (int i = dy + 5; i < dsy; i += 5)
    {

        if ((i - dy) % 20 == 0)
        {
            setlinestyle(0, 0, 2);
            line((en / 2) - 5, i, (en / 2) + 5, i);
        }
        else
        {
            setlinestyle(0, 0, 1);
            line((en / 2) - 2, i, (en / 2) + 2, i);
        }
    }
    setlinestyle(0, 0, 2);
}

int main()
{

    char dosya_adi[20];
    printf("Dosyanizin Adini Giriniz : ");
    scanf("%s", dosya_adi);

    srand(time(NULL));
    // pencere ayarlanır
    const char *title = "test";
    int en = 1200, boy = 1000;
    en = en - en % 20;
    boy = boy - boy % 20;
    nokta merkez = {(float)en / 2, (float)boy / 2};

    int n = 0;
    FILE *file;

    file = fopen(dosya_adi, "r");

    if (file == NULL)
    {
        printf("Dosya adi yanlis!\n");
        printf("program 10 saniye sonra kapanicak");
        sleep(10);
        return 0;
    }

    for (char c = getc(file); c != EOF; c = getc(file))
    {
        if (c == '\n')
            n++;
    }

    printf("Nokta sayisi:%d ", n);

    file = fopen(dosya_adi, "r");

    float x, y;
    nokta liste[n];
    int i = 0;
    while (fscanf(file, "%f %f", &x, &y) != EOF)
    {
        printf(" file: %0.3f %0.3f \n", x, y);
        nokta tmp = {x, y};
        liste[i] = tmp;
        i++;
    }

    fclose(file);

    initwindow(en, boy, title); // grafik pencere açıyoruz

    kordinat_ciz(en, boy); // kordinat düzleminin çizimi

    qsort(liste, n, sizeof(nokta), compfunc); // doğru bir spline için sıralama
    hesapla(n, liste, merkez);                // büyük dairenin çizimi
    printf("Liste boyutu:%d\n", n);

    // noktaların çizimi
    for (int i = 0; i < n; i++)
        draw_point(liste[i], merkez, 4);

    bspline(liste, merkez, n);

    while (!kbhit())
        ;

    return 0;
}
