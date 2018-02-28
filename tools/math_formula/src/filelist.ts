import { resolve } from 'path';
import { sync } from 'glob';

export default function (dir: string, ext: string): string[] {
    const ws: string = resolve(dir);
    return sync(`${ws}/Chap*/[!_]*.${ext}`);
}

export function isExist(dir: string, ext: string): string[] {
    const ws: string = resolve(dir);
    return sync(`${ws}/*/[!_]*.${ext}`);
}
