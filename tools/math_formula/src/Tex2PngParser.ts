import mathjax from './mathjax';
import filelist from './filelist';
import { isExist } from './filelist';
import { copySync, readFile,  mkdirsSync } from 'fs-extra';

type MatchTuple = [Array<string>, string, Array<[number, number]>];
export type MatchInfo = [string, Array<[string, [number, number]]>] | undefined;

export default class Tex2PngParser {
    constructor (workdir: string) {
        this.workdir = workdir;
        this.fl = filelist(this.workdir, 'md');

        let i = 0;
        const mdlist: string[] = isExist(this.workdir + 'assets/formula', 'png').sort();
        if (mdlist.length > 0) {
            console.log('Found images');
            console.log(mdlist);
            i = parseInt(mdlist[mdlist.length - 1].match(/[^/]+$/i)[0], 10) + 1;
        }
        mkdirsSync(this.workdir + 'backups');
        this.mj = new mathjax(i);
    }

    public outPng(): Promise<Array<MatchInfo>> {
        return new Promise<Array<MatchInfo>>((resolve: (value?: Array<MatchInfo>) => void, reject: (reason?: any) => void) => {
            Promise.all(this.fl.map((file: string) => this.doOutPng(file)))
                   .then((matchInfo: Array<MatchInfo>) => {
                       resolve(matchInfo.filter((ismatch: MatchInfo) => { return ismatch !== undefined; }))
                   })
                    .catch((err: any) => {
                        reject(err);
                    });
        });
    }

    private doOutPng(file: string): Promise<MatchInfo> {
        return new Promise<MatchInfo>((resolve: (value?: MatchInfo) => void, reject: (reason?: any) => void) => {
            this.readFile(file)
                .then((matches: MatchTuple) => {
                    if (matches === undefined) {
                        resolve(undefined);
                        return;
                    }
                    const filename: string = file.match(/[^/]+$/i)[0];
                    const dir: string = this.workdir + 'assets/formula/' + filename + '/';
                    
                    console.log('Working for: ' + file);
                    copySync(file, this.workdir + 'backups/' + filename);
                    mkdirsSync(dir);

                    Promise.all(matches[0].map((s: string) => this.mj.out(s.replace(/ ```mrend/g, '').replace(/```mr /g, '').replace(/\n/g, ''), dir, matches[2])))
                           .then((ret: Array<[string, [number, number]]>) => resolve([matches[1], ret]))
                           .catch((er: Error) => reject(er));
                })
                .catch((err: any) => {
                    reject(err);
                });
        });
    }

    private readFile(file: string): Promise<MatchTuple> {
        return new Promise<MatchTuple>((resolve: (value?: MatchTuple) => void, reject: (reason?: any) => void) => {
            readFile(file, { encoding: 'utf8' }, (err: Error, contents: Buffer) => {
                if (err) {
                    reject(err);
                    return;
                }
                const s: string = contents.toString('utf8', 0, contents.length);
                const regexp: RegExp = new RegExp(/```mr.+?```mrend/g);

                let matches: MatchTuple = undefined;
                let regar: Array<string> = [];
                let regexp_positions: Array<[number, number]> = [];
                
                for (let reg = regexp.exec(s); reg !== null; reg = regexp.exec(s)) {
                    regar.push(reg[0]);
                    regexp_positions.push([reg.index, regexp.lastIndex]);
                }

                if (regar.length > 0 && regexp_positions.length > 0) {
                    matches = [regar, file, regexp_positions];
                }
                resolve(matches);
            });
        });
    }

    private fl: string[];
    private workdir: string;
    private mj: mathjax;
}
