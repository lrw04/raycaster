//void translate(double x, double y, int& sx, int& sy) {
//    sx = (int) (x * BLK);
//    sy = (int) ((wh - y) * BLK);
//}

//void render2d() {
//    for (int i = 0; i < wh; i++)
//        for (int j = 0; j < ww; j++)
//            if (world[i][j] == '#')
//                drawRectangle(j * BLK + 1, (wh - i - 1) * BLK + 1, BLK - 2, BLK - 2, 0, 0xff, 0);
//            else if (world[i][j] == '.')
//                drawRectangle(j * BLK + 1, (wh - i - 1) * BLK + 1, BLK - 2, BLK - 2, 0, 0, 0xff);
//
//    for (int col = 0; col < SW; col++) {
//        double theta = pa + FOV / 2 - FOV / SW * col;
//        double dist = INFINITY, x1 = 0, y1 = 0;
//
//        double x0, y0, dx = 0, dy = 0;
//
//        // horizontal intersections
//        if (sin(theta)) {
//            y0 = sin(theta) < 0 ? floor(py) : ceil(py);
//            x0 = px + (y0 - py) / tan(theta);
//            dy = sin(theta) < 0 ? -1 : 1;
//            dx = dy / tan(theta);
//            while (!oor(floor(x0), by(y0, theta))) {
//                if (world[by(y0, theta)][(int) floor(x0)] == '#') {
//                    //dist = std::min(dist, distance(px, py, x0, y0));
//                    if (distance(px, py, x0, y0) < dist) {
//                        dist = distance(px, py, x0, y0);
//                        x1 = x0;
//                        y1 = y0;
//                    }
//                    break;
//                }
//                x0 += dx;
//                y0 += dy;
//            }
//        }
//
//        // vertical intersections
//        if (cos(theta)) {
//            x0 = cos(theta) < 0 ? floor(px) : ceil(px);
//            y0 = py + (x0 - px) * tan(theta);
//            dx = cos(theta) < 0 ? -1 : 1;
//            dy = dx * tan(theta);
//            while (!oor(bx(x0, theta), floor(y0))) {
//                if (world[(int) floor(y0)][bx(x0, theta)] == '#') {
//                    //dist = std::min(dist, distance(px, py, x0, y0));
//                    if (distance(px, py, x0, y0) < dist) {
//                        dist = distance(px, py, x0, y0);
//                        x1 = x0;
//                        y1 = y0;
//                    }
//                    break;
//                }
//                x0 += dx;
//                y0 += dy;
//            }
//        }
//
//        // Draw column
//        //draw_column(col, floor(BLK / dist), 0xff /*std::min(std::max(255.0, 255 / dist), 0.0)*/);
//
//        int spx, spy, sx1, sy1;
//        translate(px, py, spx, spy);
//        translate(x1, y1, sx1, sy1);
//        drawLine(spx, spy, sx1, sy1, 0xff, 0, 0);
//    }
//}
