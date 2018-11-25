import Tex2PngParser from './Tex2PngParser';
import { MatchInfo } from './Tex2PngParser';
import { readFile, writeFile } from 'fs';

const main : () => void = () => {
    const workdir = '../../';
    const pt = new Tex2PngParser(workdir);

    pt.outPng().then((matchInfo: Array<MatchInfo>) => {
        matchInfo.forEach((info: MatchInfo) => {
            readFile(info[0], { encoding: 'utf8' }, (err: Error, contents: Buffer) => {
                if (err) return console.log(err);

                const s: string = contents.toString('utf8', 0, contents.length);
                const flist = info[1].map((inf: [string, [number, number]]) => { return inf[0].substr(3); });
                let counter: number = 0;
                const replaced: string = s.replace(/```mr.+?```mrend/g, () => { return '![](' + flist[counter++] + ')'; });
                writeFile(info[0], replaced, { encoding: 'utf8' }, (err: Error) => { if (err) console.log(err); });
            });
        });
    }).catch((err: any) => {
        console.log(err);
    });
};

main();
